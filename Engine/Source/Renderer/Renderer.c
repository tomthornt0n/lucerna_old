/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 05 August 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static struct
{
    lcScene_t *BoundScene;
    uint32_t VertexArray, VertexBuffer, IndexBuffer;
    uint8_t *ModifiedStart, *ModifiedEnd;
} lcRenderer;

/* static void */
/* lcRendererBufferData(void) */
#define lcRendererBufferData()                                                 \
    glBufferSubData(GL_ARRAY_BUFFER,                                           \
                    0,                                                         \
                    lcRenderer.BoundScene->RenderableCount *                   \
                    sizeof(ComponentRenderable),                               \
                    lcRenderer.BoundScene->ComponentRenderable);

void
ComponentRenderableAdd(lcScene_t *scene, lcEntity_t entity,
                       float x, float y,
                       float width, float height,
                       float *colour)
{
    uint32_t index = scene->RenderableCount;

    scene->EntitySignatures[index] |= COMPONENT_RENDERABLE;
    scene->ComponentRenderable[index].Position1[0] = x - width;
    scene->ComponentRenderable[index].Position1[1] = y - height;
    memcpy(&(scene->ComponentRenderable[index].Colour1),
           colour, 4 * sizeof(float));

    scene->ComponentRenderable[index].Position2[0] = x + width;
    scene->ComponentRenderable[index].Position2[1] = y - height;
    memcpy(&(scene->ComponentRenderable[index].Colour2),
           colour, 4 * sizeof(float));

    scene->ComponentRenderable[index].Position3[0] = x + width;
    scene->ComponentRenderable[index].Position3[1] = y + height;
    memcpy(&(scene->ComponentRenderable[index].Colour3),
           colour, 4 * sizeof(float));

    scene->ComponentRenderable[index].Position4[0] = x - width;
    scene->ComponentRenderable[index].Position4[1] = y + height;
    memcpy(&(scene->ComponentRenderable[index].Colour4),
           colour, 4 * sizeof(float));


    if (scene == lcRenderer.BoundScene)
    {
        lcRendererBufferData();
    }

    scene->EntityToRenderable[entity] = index;
    ++scene->RenderableCount;
}

void
ComponentRenderableMove(lcScene_t *scene, lcEntity_t entity, 
                        float xOffset, float yOffset)
{
    if((scene->EntitySignatures[entity] & COMPONENT_RENDERABLE) == 0)
    {
        /* NOTE(tbt): Early exit if the renderable component is not in use */
        return;
    }

    uint32_t index = scene->EntityToRenderable[entity];

    scene->ComponentRenderable[index].Position1[0] += xOffset;
    scene->ComponentRenderable[index].Position1[1] += yOffset;

    scene->ComponentRenderable[index].Position2[0] += xOffset;
    scene->ComponentRenderable[index].Position2[1] += yOffset;

    scene->ComponentRenderable[index].Position3[0] += xOffset;
    scene->ComponentRenderable[index].Position3[1] += yOffset;

    scene->ComponentRenderable[index].Position4[0] += xOffset;
    scene->ComponentRenderable[index].Position4[1] += yOffset;
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
                 LC_MAX_ENTITIES * sizeof(ComponentRenderable), NULL,
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

    lcMessageBind(LC_MESSAGE_TYPE_WINDOW_RESIZE, lcRendererUpdateViewport);
}

void
lcRendererBindScene(lcScene_t *scene)
{
    lcRenderer.ModifiedStart =
        (uint8_t *)(scene->ComponentRenderable);

    lcRenderer.ModifiedEnd =
        (uint8_t *)(scene->ComponentRenderable + scene->RenderableCount);

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
lcRendererRenderToWindow(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (lcRenderer.ModifiedStart != NULL &&
        lcRenderer.ModifiedEnd   != NULL)
    {
        lcRendererBufferData();
    }

    glDrawElements(GL_TRIANGLES,
                   lcRenderer.BoundScene->RenderableCount * 6,
                   GL_UNSIGNED_INT, NULL);
}

void
lcRendererDestroy(void)
{
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

