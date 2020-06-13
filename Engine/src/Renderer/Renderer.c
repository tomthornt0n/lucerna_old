typedef struct
{
    uint32_t VertexArray, VertexBuffer, IndexBuffer;

    lcShader_t Shader;

    lcScene_t *Scene;
    lcSubset_t *Renderables;

    uint8_t *ModifiedStart, *ModifiedEnd;
} lcRenderer_t;

lcRenderer_t *lc_ActiveRenderer = NULL;

void lc_BufferRenderData(lcRenderer_t *renderer)
{
    lc_RefreshSubset(renderer->Renderables);

    uint32_t renderableCount = sb_count(renderer->Renderables->Entities);
 
    glBindBuffer(GL_ARRAY_BUFFER, renderer->VertexBuffer);
    
    lc_Renderable *renderData = calloc(renderableCount, sizeof(lc_Renderable));
    LC_SUBSET_LOOP(renderer->Renderables)
        renderData[i] = renderer->Scene->LcRenderable[entity];
    LC_END_SUBSET_LOOP
 
    ptrdiff_t offset = renderer->ModifiedStart -
             (uint8_t*)(renderer->Scene->LcRenderable);
    ptrdiff_t size = renderer->ModifiedEnd - renderer->ModifiedStart;

    LC_CORE_LOG("\n\tBuffering Vertex Data:\n\t\tOffset: %i\n\t\tSize: %i", offset, size);

    glBufferSubData(GL_ARRAY_BUFFER,
                    offset, size,
                    (uint8_t*)renderData + offset);

    free(renderData);

    renderer->ModifiedStart = NULL;
    renderer->ModifiedEnd = NULL;
}

void lc_AddRenderable(lcRenderer_t *renderer, lcEntity_t entity,
                      float x, float y,
                      float width, float height,
                      float *colour)
{
    renderer->Scene->EntitySignatures[entity] |= LC__RENDERABLE;
    renderer->Scene->LcRenderable[entity].Position1[0] = x - width;
    renderer->Scene->LcRenderable[entity].Position1[1] = y - height;
    memcpy(&(renderer->Scene->LcRenderable[entity].Colour1),
           colour, 4 * sizeof(float));

    renderer->Scene->LcRenderable[entity].Position2[0] = x + width;
    renderer->Scene->LcRenderable[entity].Position2[1] = y - height;
    memcpy(&(renderer->Scene->LcRenderable[entity].Colour2),
           colour, 4 * sizeof(float));

    renderer->Scene->LcRenderable[entity].Position3[0] = x + width;
    renderer->Scene->LcRenderable[entity].Position3[1] = y + height;
    memcpy(&(renderer->Scene->LcRenderable[entity].Colour3),
           colour, 4 * sizeof(float));

    renderer->Scene->LcRenderable[entity].Position4[0] = x - width;
    renderer->Scene->LcRenderable[entity].Position4[1] = y + height;
    memcpy(&(renderer->Scene->LcRenderable[entity].Colour4),
           colour, 4 * sizeof(float));

    if ((uint8_t*)(&(renderer->Scene->LcRenderable[entity])) <
        renderer->ModifiedStart
        || renderer->ModifiedStart == NULL)
    {
        renderer->ModifiedStart =
             (uint8_t*)(&(renderer->Scene->LcRenderable[entity]));
    }
    if ((uint8_t*)(&(renderer->Scene->LcRenderable[entity]) + 1) >
        renderer->ModifiedEnd
        || renderer->ModifiedEnd == NULL)
    {
        renderer->ModifiedEnd =
             (uint8_t*)(&(renderer->Scene->LcRenderable[entity]) + 1);
    }
}

void lc_MoveRenderable(lcRenderer_t *renderer, lcEntity_t entity,
                       float xOffset, float yOffset)
{
    renderer->Scene->LcRenderable[entity].Position1[0] += xOffset;
    renderer->Scene->LcRenderable[entity].Position1[1] += yOffset;

    renderer->Scene->LcRenderable[entity].Position2[0] += xOffset;
    renderer->Scene->LcRenderable[entity].Position2[1] += yOffset;

    renderer->Scene->LcRenderable[entity].Position3[0] += xOffset;
    renderer->Scene->LcRenderable[entity].Position3[1] += yOffset;

    renderer->Scene->LcRenderable[entity].Position4[0] += xOffset;
    renderer->Scene->LcRenderable[entity].Position4[1] += yOffset;

    if ((uint8_t*)(&(renderer->Scene->LcRenderable[entity])) <
        renderer->ModifiedStart
        || renderer->ModifiedStart == NULL)
    {
        renderer->ModifiedStart =
             (uint8_t*)(&(renderer->Scene->LcRenderable[entity]));
    }
    if ((uint8_t*)(&(renderer->Scene->LcRenderable[entity]) + 1) >
        renderer->ModifiedEnd
        || renderer->ModifiedEnd == NULL)
    {
        renderer->ModifiedEnd =
             (uint8_t*)(&(renderer->Scene->LcRenderable[entity]) + 1);
    }
}

void lc_CreateRenderer(lcRenderer_t *output,
                       lcScene_t *scene, lcShader_t shader)
{
    output->Scene = scene;
    output->Shader = shader;

    output->ModifiedStart = NULL;
    output->ModifiedEnd = NULL;

    glGenVertexArrays(1, &(output->VertexArray));
    glBindVertexArray(output->VertexArray);

    glGenBuffers(1, &(output->VertexBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, output->VertexBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), NULL);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (const void*)(2 * sizeof(float)));

    glBufferData(GL_ARRAY_BUFFER,
                 LC_MAX_ENTITIES * sizeof(lc_Renderable), NULL,
                 GL_DYNAMIC_DRAW);

    glGenBuffers(1, &(output->IndexBuffer));
    unsigned int *indices = calloc(LC_MAX_ENTITIES * 6, sizeof(uint32_t));
    unsigned int offset = 0;

    int i;
    for (i = 0; i < LC_MAX_ENTITIES * 6; i += 6)
    {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;

        indices[i + 3] = 2 + offset;
        indices[i + 4] = 3 + offset;
        indices[i + 5] = 0 + offset;

        offset += 4;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, output->IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 LC_MAX_ENTITIES * 6 * sizeof(uint32_t), indices,
                 GL_STATIC_DRAW);

    free(indices);

    output->Renderables = lc_CreateSubset(scene);
    lc_SetSubsetSignature(output->Renderables, LC__RENDERABLE);
}

void lc_Render(lcRenderer_t *renderer)
{
    if (lc_ActiveRenderer != renderer)
    {
        glBindVertexArray(renderer->VertexArray);
        glUseProgram(renderer->Shader);
        lc_ActiveRenderer = renderer;
    }

    glClearColor(0.27f, 0.28f, 0.35f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (lc_ActiveRenderer->ModifiedStart != NULL ||
        lc_ActiveRenderer->ModifiedEnd != NULL)
    {
        lc_BufferRenderData(lc_ActiveRenderer);
    }

    glDrawElements(GL_TRIANGLES,
                   sb_count(lc_ActiveRenderer->Renderables->Entities) * 6,
                   GL_UNSIGNED_INT, NULL);
}

void lc_DestroyRenderer(lcRenderer_t *renderer)
{
    lc_DestroySubset(renderer->Renderables);
    glDeleteBuffers(1, &(renderer->VertexBuffer));
    glDeleteBuffers(1, &(renderer->IndexBuffer));
    glDeleteVertexArrays(1, &(renderer->VertexArray));
}
