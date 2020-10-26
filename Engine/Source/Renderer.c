/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 24 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

struct
{
    lcScene_t *BoundScene;
    uint32_t VertexArray, VertexBuffer, IndexBuffer;
    uint8_t *ModifiedStart, *ModifiedEnd;
} lcRenderer;

#define lcRendererBufferData()                                                 \
    gl.BufferSubData(GL_ARRAY_BUFFER,                                          \
                     0,                                                        \
                     lcRenderer.BoundScene->RenderableCount *                  \
                     sizeof(ComponentRenderable),                              \
                     lcRenderer.BoundScene->ComponentRenderable);

void
ComponentRenderableAdd(lcScene_t *scene, lcEntity_t entity,
                       float x, float y,
                       float width, float height,
                       float *colour,
                       lcSprite_t *texture)
{
    uint32_t index;

    index = scene->RenderableCount;

    scene->EntitySignatures[entity] |= COMPONENT_RENDERABLE;

    scene->ComponentRenderable[index].Position1[0] = x - width;
    scene->ComponentRenderable[index].Position1[1] = y - height;
    scene->ComponentRenderable[index].TexCoords1[0] = texture->Min[0];
    scene->ComponentRenderable[index].TexCoords1[1] = texture->Min[1];
    memcpy(&(scene->ComponentRenderable[index].Colour1),
           colour, 4 * sizeof(float));

    scene->ComponentRenderable[index].Position2[0] = x + width;
    scene->ComponentRenderable[index].Position2[1] = y - height;
    scene->ComponentRenderable[index].TexCoords2[0] = texture->Max[0];
    scene->ComponentRenderable[index].TexCoords2[1] = texture->Min[1];
    memcpy(&(scene->ComponentRenderable[index].Colour2),
           colour, 4 * sizeof(float));

    scene->ComponentRenderable[index].Position3[0] = x + width;
    scene->ComponentRenderable[index].Position3[1] = y + height;
    scene->ComponentRenderable[index].TexCoords3[0] = texture->Max[0];
    scene->ComponentRenderable[index].TexCoords3[1] = texture->Max[1];
    memcpy(&(scene->ComponentRenderable[index].Colour3),
           colour, 4 * sizeof(float));

    scene->ComponentRenderable[index].Position4[0] = x - width;
    scene->ComponentRenderable[index].Position4[1] = y + height;
    scene->ComponentRenderable[index].TexCoords4[0] = texture->Min[0];
    scene->ComponentRenderable[index].TexCoords4[1] = texture->Max[1];
    memcpy(&(scene->ComponentRenderable[index].Colour4),
           colour, 4 * sizeof(float));


    if (scene == lcRenderer.BoundScene)
    {
        lcRendererBufferData();
    }

    scene->EntityToRenderable[entity] = index;
    ++(scene->RenderableCount);
}

void
ComponentRenderableMove(lcScene_t *scene, lcEntity_t entity, 
                        float xOffset, float yOffset)
{
    uint32_t index;

    if (!(scene->EntitySignatures[entity] & COMPONENT_RENDERABLE))
    {
        LC_CORE_LOG_WARN("Attempting to move unused renderable component.");
        return;
    }

    index = scene->EntityToRenderable[entity];

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
lcRendererUpdateViewport(lcGenericMessage_t *message)
{
    lcWindowResizeMessage_t *resize;
    resize = (lcWindowResizeMessage_t *)message;

    gl.Viewport(0, 0,
                resize->Width,
                resize->Height);
}

static void
lcRendererInit(void)
{
    uint32_t *indices;
    uint32_t offset;
    int i;

    lcRendererBoundShader = -1;
    lcRenderer.ModifiedStart = NULL;
    lcRenderer.ModifiedEnd = NULL;

    gl.GenVertexArrays(1, &(lcRenderer.VertexArray));
    gl.BindVertexArray(lcRenderer.VertexArray);

    gl.GenBuffers(1, &(lcRenderer.VertexBuffer));
    gl.BindBuffer(GL_ARRAY_BUFFER, lcRenderer.VertexBuffer);

    gl.EnableVertexAttribArray(0);
    gl.VertexAttribPointer(0,
                           2,
                           GL_FLOAT,
                           GL_FALSE,
                           8 * sizeof(float),
                           NULL);

    gl.EnableVertexAttribArray(1);
    gl.VertexAttribPointer(1,
                           4,
                           GL_FLOAT,
                           GL_FALSE,
                           8 * sizeof(float),
                           (const void*)(2 * sizeof(float)));

    gl.EnableVertexAttribArray(2);
    gl.VertexAttribPointer(2,
                           2,
                           GL_FLOAT,
                           GL_FALSE,
                           8 * sizeof(float),
                           (const void*)(6 * sizeof(float)));

    gl.BufferData(GL_ARRAY_BUFFER,
                  LC_MAX_ENTITIES * sizeof(ComponentRenderable), NULL,
                  GL_DYNAMIC_DRAW);

    gl.GenBuffers(1, &(lcRenderer.IndexBuffer));
    indices = calloc(LC_MAX_ENTITIES * 6, sizeof(uint32_t));
    offset = 0;

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

    gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, lcRenderer.IndexBuffer);
    gl.BufferData(GL_ELEMENT_ARRAY_BUFFER,
                  LC_MAX_ENTITIES * 6 * sizeof(uint32_t), indices,
                  GL_STATIC_DRAW);

    free(indices);

    gl.ClearColor(1.0f, 0.0f, 0.0f, 0.0f);

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
    uint32_t windowSize[2];

    lcRendererBoundShader = shader;
    gl.UseProgram(shader);

    lcCamera.UniformLocation = gl.GetUniformLocation(shader,
                                                     LC_CAMERA_UNIFORM_NAME);
    if (lcCamera.UniformLocation == -1)
    {
        LC_CORE_LOG_WARN("Uniform '%s' does not exist!",
                         LC_CAMERA_UNIFORM_NAME);
    }

    lcWindowGetSize(windowSize);
    lcMessageEmit(lcWindowResizeMessageCreate(windowSize[0], windowSize[1]));
}


void
lcRendererRenderToWindow(void)
{
    lcGLGetErrors();

    gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (lcRenderer.ModifiedStart != NULL &&
        lcRenderer.ModifiedEnd   != NULL)
    {
        lcRendererBufferData();
    }
    
    gl.DrawElements(GL_TRIANGLES,
                    lcRenderer.BoundScene->RenderableCount * 6,
                    GL_UNSIGNED_INT, NULL);
}


static void
lcRendererDestroy(void)
{
    gl.DeleteBuffers(1, &(lcRenderer.VertexBuffer));
    gl.DeleteBuffers(1, &(lcRenderer.IndexBuffer));
    gl.DeleteVertexArrays(1, &(lcRenderer.VertexArray));
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

