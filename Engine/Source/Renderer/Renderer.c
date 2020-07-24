struct
{
    uint32_t VertexArray, VertexBuffer, IndexBuffer;

    lcScene_t *BoundScene;

    lcSubset_t *Renderables;

    uint8_t *ModifiedStart, *ModifiedEnd;
} _lc_Renderer;

void
_lc_RendererBufferData(void)
{
    if (_lc_Renderer.ModifiedStart == NULL ||
        _lc_Renderer.ModifiedEnd == NULL)
    {
        return;
    }

    lc_SubsetRefresh(_lc_Renderer.Renderables);

    uint32_t renderableCount = LC_LIST_LEN(_lc_Renderer.Renderables->Entities);
 
    glBindBuffer(GL_ARRAY_BUFFER, _lc_Renderer.VertexBuffer);
    
    lc_Renderable *renderData = calloc(renderableCount, sizeof(lc_Renderable));
    LC_SUBSET_LOOP(_lc_Renderer.Renderables)
        renderData[i] = _lc_Renderer.BoundScene->LcRenderable[entity];
    LC_END_SUBSET_LOOP
 
    ptrdiff_t offset = _lc_Renderer.ModifiedStart -
             (uint8_t*)(_lc_Renderer.BoundScene->LcRenderable);
    ptrdiff_t size = _lc_Renderer.ModifiedEnd - _lc_Renderer.ModifiedStart;

    glBufferSubData(GL_ARRAY_BUFFER,
                    offset, size,
                    (uint8_t*)renderData + offset);

    free(renderData);

    _lc_Renderer.ModifiedStart = NULL;
    _lc_Renderer.ModifiedEnd = NULL;
}

void
lc_AddRenderable(lcEntity_t entity, lcScene_t *scene,
                 float x, float y,
                 float width, float height,
                 float *colour)
{
    scene->EntitySignatures[entity] |= LC__RENDERABLE;
    scene->LcRenderable[entity].Position1[0] = x - width;
    scene->LcRenderable[entity].Position1[1] = y - height;
    memcpy(&(scene->LcRenderable[entity].Colour1),
           colour, 4 * sizeof(float));

    scene->LcRenderable[entity].Position2[0] = x + width;
    scene->LcRenderable[entity].Position2[1] = y - height;
    memcpy(&(scene->LcRenderable[entity].Colour2),
           colour, 4 * sizeof(float));

    scene->LcRenderable[entity].Position3[0] = x + width;
    scene->LcRenderable[entity].Position3[1] = y + height;
    memcpy(&(scene->LcRenderable[entity].Colour3),
           colour, 4 * sizeof(float));

    scene->LcRenderable[entity].Position4[0] = x - width;
    scene->LcRenderable[entity].Position4[1] = y + height;
    memcpy(&(scene->LcRenderable[entity].Colour4),
           colour, 4 * sizeof(float));

    if (((uint8_t*)(&(_lc_Renderer.BoundScene->LcRenderable[entity])) <
        _lc_Renderer.ModifiedStart
        || _lc_Renderer.ModifiedStart == NULL)
        && _lc_Renderer.BoundScene == scene)
    {
        _lc_Renderer.ModifiedStart =
             (uint8_t*)(&(_lc_Renderer.BoundScene->LcRenderable[entity]));
    }
    if (((uint8_t*)(&(_lc_Renderer.BoundScene->LcRenderable[entity]) + 1) >
        _lc_Renderer.ModifiedEnd
        || _lc_Renderer.ModifiedEnd == NULL)
        && _lc_Renderer.BoundScene == scene)
    {
        _lc_Renderer.ModifiedEnd =
             (uint8_t*)(&(_lc_Renderer.BoundScene->LcRenderable[entity]) + 1);
    }
}

void
lc_RenderableMove(lcEntity_t entity, lcScene_t *scene,
                  float xOffset, float yOffset)
{
    LC_ASSERT((scene->EntitySignatures[entity] & LC__RENDERABLE)
              == LC__RENDERABLE,
              "Entity does not have renderable component!");

    scene->LcRenderable[entity].Position1[0] += xOffset;
    scene->LcRenderable[entity].Position1[1] += yOffset;

    scene->LcRenderable[entity].Position2[0] += xOffset;
    scene->LcRenderable[entity].Position2[1] += yOffset;

    scene->LcRenderable[entity].Position3[0] += xOffset;
    scene->LcRenderable[entity].Position3[1] += yOffset;

    scene->LcRenderable[entity].Position4[0] += xOffset;
    scene->LcRenderable[entity].Position4[1] += yOffset;

    if (((uint8_t*)(&(_lc_Renderer.BoundScene->LcRenderable[entity])) <
        _lc_Renderer.ModifiedStart
        || _lc_Renderer.ModifiedStart == NULL)
        && _lc_Renderer.BoundScene == scene)
    {
        _lc_Renderer.ModifiedStart =
             (uint8_t*)(&(_lc_Renderer.BoundScene->LcRenderable[entity]));
    }
    if (((uint8_t*)(&(_lc_Renderer.BoundScene->LcRenderable[entity]) + 1) >
        _lc_Renderer.ModifiedEnd
        || _lc_Renderer.ModifiedEnd == NULL)
        && _lc_Renderer.BoundScene == scene)
    {
        _lc_Renderer.ModifiedEnd =
             (uint8_t*)(&(_lc_Renderer.BoundScene->LcRenderable[entity]) + 1);
    }
}

void
_lc_RendererUpdateViewport(lcMessage_t message)
{
    glViewport(0, 0,
               message.WindowResize.Width,
               message.WindowResize.Height);
}

void
lc_RendererInit(void)
{
    _lc_Renderer.ModifiedStart = NULL;
    _lc_Renderer.ModifiedEnd = NULL;

    glGenVertexArrays(1, &(_lc_Renderer.VertexArray));
    glBindVertexArray(_lc_Renderer.VertexArray);

    glGenBuffers(1, &(_lc_Renderer.VertexBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, _lc_Renderer.VertexBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), NULL);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (const void*)(2 * sizeof(float)));

    glBufferData(GL_ARRAY_BUFFER,
                 LC_MAX_ENTITIES * sizeof(lc_Renderable), NULL,
                 GL_DYNAMIC_DRAW);

    glGenBuffers(1, &(_lc_Renderer.IndexBuffer));
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _lc_Renderer.IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 LC_MAX_ENTITIES * 6 * sizeof(uint32_t), indices,
                 GL_STATIC_DRAW);

    free(indices);

    glClearColor(0.27f, 0.28f, 0.35f, 0.5f);

    _lc_Renderer.Renderables = NULL;

    lc_MessageBind(LC_MESSAGE_TYPE_WINDOW_RESIZE, _lc_RendererUpdateViewport);
}

void
lc_RendererBindScene(lcScene_t *scene)
{
    if (_lc_Renderer.Renderables != NULL)
        lc_SubsetDestroy(_lc_Renderer.Renderables);

    _lc_Renderer.Renderables = lc_SubsetCreate(scene);
    lc_SubsetSetSignature(_lc_Renderer.Renderables, LC__RENDERABLE);
    lc_SubsetRefresh(_lc_Renderer.Renderables);

    _lc_Renderer.ModifiedStart =
        (uint8_t *)(scene->LcRenderable);

    _lc_Renderer.ModifiedEnd =
        (uint8_t *)
        (&(scene->LcRenderable[
            LC_LIST_LEN(_lc_Renderer.Renderables->Entities)
        ]));

    _lc_Renderer.BoundScene = scene;

    _lc_RendererBufferData();
}

void
lc_RendererBindShader(lcShader_t shader)
{
    glUseProgram(shader);
    _lc_RendererBoundShader = shader;

    lc_Camera._UniformLocation = glGetUniformLocation(
        shader,
        lc_Camera._UniformName
    );

    glUniformMatrix4fv(lc_Camera._UniformLocation,
                       1, GL_FALSE,
                       lc_Camera._ViewProjectionMatrix);
}

void
lc_RendererRender(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_lc_Renderer.ModifiedStart != NULL &&
        _lc_Renderer.ModifiedEnd   != NULL)
    {
        _lc_RendererBufferData();
    }

    glDrawElements(GL_TRIANGLES,
                   LC_LIST_LEN(_lc_Renderer.Renderables->Entities) * 6,
                   GL_UNSIGNED_INT, NULL);
}

void
lc_RendererDestroy(void)
{
    lc_SubsetDestroy(_lc_Renderer.Renderables);
    glDeleteBuffers(1, &(_lc_Renderer.VertexBuffer));
    glDeleteBuffers(1, &(_lc_Renderer.IndexBuffer));
    glDeleteVertexArrays(1, &(_lc_Renderer.VertexArray));
}
