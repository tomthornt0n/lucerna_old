/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 27 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

internal struct
{
    lcScene_t *BoundScene;
    u32 VertexArray, VertexBuffer, IndexBuffer;
    u8 *ModifiedStart, *ModifiedEnd;
} _lcRenderer;

#define _lcRendererBufferData()                                                \
    gl.BufferSubData(GL_ARRAY_BUFFER,                                          \
                     0,                                                        \
                     _lcRenderer.BoundScene->RenderableCount *                 \
                     sizeof(ComponentRenderable),                              \
                     _lcRenderer.BoundScene->ComponentRenderable);

void
ComponentRenderableAdd(lcScene_t *scene, lcEntity_t entity,
                       f32 x, f32 y,
                       f32 width, f32 height,
                       f32 *colour,
                       lcSprite_t *texture)
{
    u32 index;

    index = scene->RenderableCount;

    scene->EntitySignatures[entity] |= COMPONENT_RENDERABLE;

    scene->ComponentRenderable[index].Position1[0] = x - width;
    scene->ComponentRenderable[index].Position1[1] = y - height;
    scene->ComponentRenderable[index].TexCoords1[0] = texture->Min[0];
    scene->ComponentRenderable[index].TexCoords1[1] = texture->Min[1];
    memcpy(&(scene->ComponentRenderable[index].Colour1),
           colour, 4 * sizeof(f32));

    scene->ComponentRenderable[index].Position2[0] = x + width;
    scene->ComponentRenderable[index].Position2[1] = y - height;
    scene->ComponentRenderable[index].TexCoords2[0] = texture->Max[0];
    scene->ComponentRenderable[index].TexCoords2[1] = texture->Min[1];
    memcpy(&(scene->ComponentRenderable[index].Colour2),
           colour, 4 * sizeof(f32));

    scene->ComponentRenderable[index].Position3[0] = x + width;
    scene->ComponentRenderable[index].Position3[1] = y + height;
    scene->ComponentRenderable[index].TexCoords3[0] = texture->Max[0];
    scene->ComponentRenderable[index].TexCoords3[1] = texture->Max[1];
    memcpy(&(scene->ComponentRenderable[index].Colour3),
           colour, 4 * sizeof(f32));

    scene->ComponentRenderable[index].Position4[0] = x - width;
    scene->ComponentRenderable[index].Position4[1] = y + height;
    scene->ComponentRenderable[index].TexCoords4[0] = texture->Min[0];
    scene->ComponentRenderable[index].TexCoords4[1] = texture->Max[1];
    memcpy(&(scene->ComponentRenderable[index].Colour4),
           colour, 4 * sizeof(f32));


    if (scene == _lcRenderer.BoundScene)
    {
        _lcRendererBufferData();
    }

    scene->EntityToRenderable[entity] = index;
    ++(scene->RenderableCount);
}

void
ComponentRenderableMove(lcScene_t *scene,
                        lcEntity_t entity, 
                        f32 xOffset,
                        f32 yOffset)
{
    u32 index;

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

internal void
_lcRendererUpdateViewport(lcGenericMessage_t *message)
{
    lcWindowResizeMessage_t *resize;
    resize = (lcWindowResizeMessage_t *)message;

    gl.Viewport(0, 0,
                resize->Width,
                resize->Height);
}

internal void
_lcRendererInit(void)
{
    u32 *indices;
    u32 offset;
    int i;

    _lcRendererBoundShader = -1;
    _lcRenderer.ModifiedStart = NULL;
    _lcRenderer.ModifiedEnd = NULL;

    gl.GenVertexArrays(1, &(_lcRenderer.VertexArray));
    gl.BindVertexArray(_lcRenderer.VertexArray);

    gl.GenBuffers(1, &(_lcRenderer.VertexBuffer));
    gl.BindBuffer(GL_ARRAY_BUFFER, _lcRenderer.VertexBuffer);

    gl.EnableVertexAttribArray(0);
    gl.VertexAttribPointer(0,
                           2,
                           GL_FLOAT,
                           GL_FALSE,
                           8 * sizeof(f32),
                           NULL);

    gl.EnableVertexAttribArray(1);
    gl.VertexAttribPointer(1,
                           4,
                           GL_FLOAT,
                           GL_FALSE,
                           8 * sizeof(f32),
                           (const void*)(2 * sizeof(f32)));

    gl.EnableVertexAttribArray(2);
    gl.VertexAttribPointer(2,
                           2,
                           GL_FLOAT,
                           GL_FALSE,
                           8 * sizeof(f32),
                           (const void*)(6 * sizeof(f32)));

    gl.BufferData(GL_ARRAY_BUFFER,
                  LC_MAX_ENTITIES * sizeof(ComponentRenderable), NULL,
                  GL_DYNAMIC_DRAW);

    gl.GenBuffers(1, &(_lcRenderer.IndexBuffer));
    indices = malloc(LC_MAX_ENTITIES * 6 * sizeof(u32));
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

    gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, _lcRenderer.IndexBuffer);
    gl.BufferData(GL_ELEMENT_ARRAY_BUFFER,
                  LC_MAX_ENTITIES * 6 * sizeof(u32), indices,
                  GL_STATIC_DRAW);

    free(indices);

    gl.ClearColor(1.0f, 0.0f, 0.0f, 0.0f);

    lcMessageBind(LC_MESSAGE_TYPE_WINDOW_RESIZE, _lcRendererUpdateViewport);
}

void
lcRendererBindScene(lcScene_t *scene)
{
    _lcRenderer.ModifiedStart =
        (u8 *)(scene->ComponentRenderable);

    _lcRenderer.ModifiedEnd =
        (u8 *)(scene->ComponentRenderable + scene->RenderableCount);

    _lcRenderer.BoundScene = scene;

    _lcRendererBufferData();
}

void
lcRendererBindShader(lcShader_t shader)
{
    u32 windowSize[2];

    _lcRendererBoundShader = shader;
    gl.UseProgram(shader);

    _lcCamera.UniformLocation = gl.GetUniformLocation(shader,
                                                     LC_CAMERA_UNIFORM_NAME);
    if (_lcCamera.UniformLocation == -1)
    {
        LC_CORE_LOG_WARN("Uniform '%s' does not exist!",
                         LC_CAMERA_UNIFORM_NAME);
    }

    lcWindowGetSize(windowSize);
    _lcMessageEmit(_lcWindowResizeMessageCreate(windowSize[0], windowSize[1]));
}


void
lcRendererRenderToWindow(void)
{
    lcGLGetErrors();

    gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_lcRenderer.ModifiedStart != NULL &&
        _lcRenderer.ModifiedEnd   != NULL)
    {
        _lcRendererBufferData();
    }
    
    gl.DrawElements(GL_TRIANGLES,
                    _lcRenderer.BoundScene->RenderableCount * 6,
                    GL_UNSIGNED_INT, NULL);
}


internal void
_lcRendererDestroy(void)
{
    gl.DeleteBuffers(1, &(_lcRenderer.VertexBuffer));
    gl.DeleteBuffers(1, &(_lcRenderer.IndexBuffer));
    gl.DeleteVertexArrays(1, &(_lcRenderer.VertexArray));
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

