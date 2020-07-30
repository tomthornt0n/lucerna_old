/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 30 July 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static struct
{
    uint32_t VertexArray, VertexBuffer, IndexBuffer;

    lcScene_t *BoundScene;

    lcSubset_t *Renderables;

    uint8_t *ModifiedStart, *ModifiedEnd;
} lcRenderer;

static void
lcRendererBufferData(void)
{
    if (lcRenderer.ModifiedStart == NULL ||
        lcRenderer.ModifiedEnd == NULL)
    {
        return;
    }

    lcSubsetRefresh(lcRenderer.Renderables);

    uint32_t renderableCount = LC_LIST_LEN(lcRenderer.Renderables->Entities);
 
    glBindBuffer(GL_ARRAY_BUFFER, lcRenderer.VertexBuffer);
    
    lcRenderable *renderData = calloc(renderableCount, sizeof(lcRenderable));
    LC_SUBSET_LOOP(lcRenderer.Renderables)
        renderData[i] = lcRenderer.BoundScene->LcRenderable[entity];
    LC_END_SUBSET_LOOP
 
    ptrdiff_t offset = lcRenderer.ModifiedStart -
             (uint8_t*)(lcRenderer.BoundScene->LcRenderable);
    ptrdiff_t size = lcRenderer.ModifiedEnd - lcRenderer.ModifiedStart;

    glBufferSubData(GL_ARRAY_BUFFER,
                    offset, size,
                    (uint8_t*)renderData + offset);

    free(renderData);

    lcRenderer.ModifiedStart = NULL;
    lcRenderer.ModifiedEnd = NULL;
}

void
lcAddRenderable(lcEntity_t entity, lcScene_t *scene,
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

    if (((uint8_t*)(&(lcRenderer.BoundScene->LcRenderable[entity])) <
        lcRenderer.ModifiedStart
        || lcRenderer.ModifiedStart == NULL)
        && lcRenderer.BoundScene == scene)
    {
        lcRenderer.ModifiedStart =
             (uint8_t*)(&(lcRenderer.BoundScene->LcRenderable[entity]));
    }
    if (((uint8_t*)(&(lcRenderer.BoundScene->LcRenderable[entity]) + 1) >
        lcRenderer.ModifiedEnd
        || lcRenderer.ModifiedEnd == NULL)
        && lcRenderer.BoundScene == scene)
    {
        lcRenderer.ModifiedEnd =
             (uint8_t*)(&(lcRenderer.BoundScene->LcRenderable[entity]) + 1);
    }
}

void
lcRenderableMove(lcEntity_t entity, lcScene_t *scene,
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

    if (((uint8_t*)(&(lcRenderer.BoundScene->LcRenderable[entity])) <
        lcRenderer.ModifiedStart
        || lcRenderer.ModifiedStart == NULL)
        && lcRenderer.BoundScene == scene)
    {
        lcRenderer.ModifiedStart =
             (uint8_t*)(&(lcRenderer.BoundScene->LcRenderable[entity]));
    }
    if (((uint8_t*)(&(lcRenderer.BoundScene->LcRenderable[entity]) + 1) >
        lcRenderer.ModifiedEnd
        || lcRenderer.ModifiedEnd == NULL)
        && lcRenderer.BoundScene == scene)
    {
        lcRenderer.ModifiedEnd =
             (uint8_t*)(&(lcRenderer.BoundScene->LcRenderable[entity]) + 1);
    }
}

static void
lcRendererUpdateViewport(lcMessage_t message)
{
    glViewport(0, 0,
               message.WindowResize.Width,
               message.WindowResize.Height);
}

void
lcRendererInit(void)
{
    lcRenderer.ModifiedStart = NULL;
    lcRenderer.ModifiedEnd = NULL;

    glGenVertexArrays(1, &(lcRenderer.VertexArray));
    glBindVertexArray(lcRenderer.VertexArray);

    glGenBuffers(1, &(lcRenderer.VertexBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, lcRenderer.VertexBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), NULL);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (const void*)(2 * sizeof(float)));

    glBufferData(GL_ARRAY_BUFFER,
                 LC_MAX_ENTITIES * sizeof(lcRenderable), NULL,
                 GL_DYNAMIC_DRAW);

    glGenBuffers(1, &(lcRenderer.IndexBuffer));
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lcRenderer.IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 LC_MAX_ENTITIES * 6 * sizeof(uint32_t), indices,
                 GL_STATIC_DRAW);

    free(indices);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    lcRenderer.Renderables = NULL;

    lcMessageBind(LC_MESSAGE_TYPE_WINDOW_RESIZE, lcRendererUpdateViewport);
}

void
lcRendererBindScene(lcScene_t *scene)
{
    if (lcRenderer.Renderables != NULL)
        lcSubsetDestroy(lcRenderer.Renderables);

    lcRenderer.Renderables = lcSubsetCreate(scene);
    lcSubsetSetSignature(lcRenderer.Renderables, LC__RENDERABLE);
    lcSubsetRefresh(lcRenderer.Renderables);

    lcRenderer.ModifiedStart =
        (uint8_t *)(scene->LcRenderable);

    lcRenderer.ModifiedEnd =
        (uint8_t *)
        (&(scene->LcRenderable[
            LC_LIST_LEN(lcRenderer.Renderables->Entities)
        ]));

    lcRenderer.BoundScene = scene;

    lcRendererBufferData();
}

void
lcRendererBindShader(lcShader_t shader)
{
    glUseProgram(shader);
    lcRendererBoundShader = shader;

    lcCamera.UniformLocation = glGetUniformLocation(shader,
                                                    lcCamera.UniformName);

    glUniformMatrix4fv(lcCamera.UniformLocation,
                       1, GL_FALSE,
                       lcCamera.ViewProjectionMatrix);
}

void
lcRendererRender(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (lcRenderer.ModifiedStart != NULL &&
        lcRenderer.ModifiedEnd   != NULL)
    {
        lcRendererBufferData();
    }

    glDrawElements(GL_TRIANGLES,
                   LC_LIST_LEN(lcRenderer.Renderables->Entities) * 6,
                   GL_UNSIGNED_INT, NULL);
}

void
lcRendererDestroy(void)
{
    lcSubsetDestroy(lcRenderer.Renderables);
    glDeleteBuffers(1, &(lcRenderer.VertexBuffer));
    glDeleteBuffers(1, &(lcRenderer.IndexBuffer));
    glDeleteVertexArrays(1, &(lcRenderer.VertexArray));
}

/*
MIT License

Copyright (c) 2020 Tom Thornton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

