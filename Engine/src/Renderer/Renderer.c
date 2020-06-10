uint32_t lc_VertexArray, lc_VertexBuffer, lc_IndexBuffer;

lc_Scene* lc_RenderedScene;
lc_Subset* lc_Renderables;

bool lc_RenderDataUpToDate;

void lc_BufferRenderData(void)
{
    lc_RefreshSubset(lc_Renderables);

    uint32_t renderableCount = sb_count(lc_Renderables->Entities);
 
    glBindBuffer(GL_ARRAY_BUFFER, lc_VertexBuffer);
    
    LC_CORE_INFO("Buffering %i bytes of vertex data (%i quads).", renderableCount * sizeof(lc_Renderable), renderableCount);

    lc_Renderable* renderData = calloc(renderableCount, sizeof(lc_Renderable));
    LC_SUBSET_LOOP(lc_Renderables)
        renderData[i] = lc_Renderables->Parent->LcRenderable[entity];
    LC_END_SUBSET_LOOP
 
    glBufferSubData(GL_ARRAY_BUFFER, 0, renderableCount * sizeof(lc_Renderable), renderData);
    free(renderData);

    lc_RenderDataUpToDate = true;
}

void lc_AddRenderable(lc_Scene* scene, lc_Entity entity, float x, float y, float width, float height, float* colour)
{
    LC_ASSERT(scene == lc_RenderedScene, "ERROR: Entity not in renderable scene!");

    scene->EntitySignatures[entity] |= LC__RENDERABLE;
    LC_CORE_LOG("Adding renderable component. New signature is: 0x%x", scene->EntitySignatures[entity]);

    scene->LcRenderable[entity].Position1[0] = x - width;
    scene->LcRenderable[entity].Position1[1] = y - height;
    memcpy(&(scene->LcRenderable[entity].Colour1), colour, 4 * sizeof(float));

    scene->LcRenderable[entity].Position2[0] = x + width;
    scene->LcRenderable[entity].Position2[1] = y - height;
    memcpy(&(scene->LcRenderable[entity].Colour2), colour, 4 * sizeof(float));

    scene->LcRenderable[entity].Position3[0] = x + width;
    scene->LcRenderable[entity].Position3[1] = y + height;
    memcpy(&(scene->LcRenderable[entity].Colour3), colour, 4 * sizeof(float));

    scene->LcRenderable[entity].Position4[0] = x - width;
    scene->LcRenderable[entity].Position4[1] = y + height;
    memcpy(&(scene->LcRenderable[entity].Colour4), colour, 4 * sizeof(float));

    lc_RenderDataUpToDate = false;
}

void lc_InitRenderer(lc_Scene* scene)
{
    lc_RenderedScene = scene;

    lc_RenderDataUpToDate = true;

    glGenVertexArrays(1, &lc_VertexArray);
    glBindVertexArray(lc_VertexArray);

    glGenBuffers(1, &lc_VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, lc_VertexBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)(2 * sizeof(float)));

    glBufferData(GL_ARRAY_BUFFER, LC_MAX_ENTITIES * sizeof(lc_Renderable), NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &lc_IndexBuffer);
    unsigned int* indices = calloc(LC_MAX_ENTITIES * 6, sizeof(uint32_t));
    unsigned int offset   = 0;
    for (size_t i = 0; i < LC_MAX_ENTITIES * 6; i += 6)
    {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;

        indices[i + 3] = 2 + offset;
        indices[i + 4] = 3 + offset;
        indices[i + 5] = 0 + offset;

        offset += 4;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lc_IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, LC_MAX_ENTITIES * 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    free(indices);

    lc_Renderables = lc_CreateSubset(scene);
    lc_SetSubsetSignature(lc_Renderables, LC__RENDERABLE);
}

void lc_Render(lc_Shader shader)
{
    glClearColor(0.27f, 0.28f, 0.35f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);

    if (!lc_RenderDataUpToDate)
        lc_BufferRenderData();

    glDrawElements(GL_TRIANGLES, sb_count(lc_Renderables->Entities) * 6, GL_UNSIGNED_INT, NULL);
}

void lc_DestroyRenderer(void)
{
    lc_DestroySubset(lc_Renderables);
    glDeleteBuffers(1, &lc_VertexBuffer);
    glDeleteBuffers(1, &lc_IndexBuffer);
    glDeleteVertexArrays(1, &lc_VertexArray);
}
