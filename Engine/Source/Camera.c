/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 28 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_CAMERA_UNIFORM_NAME "u_ViewProjectionMatrix"

internal struct
{
    GLint UniformLocation;

    f32 *ProjectionMatrix;
    f32 *ViewMatrix;
    f32 *ViewProjectionMatrix;

    f32 Position[2];
} _lcCamera;

internal void
lcCameraRecalculateViewProjectionMatrix(void)
{
    lcMatrix4Multiply(_lcCamera.ViewProjectionMatrix,
                      _lcCamera.ProjectionMatrix,
                      _lcCamera.ViewMatrix);

    if (_lcCamera.UniformLocation != -1)
    {
        gl.UniformMatrix4fv(_lcCamera.UniformLocation,
                            1,
                            GL_FALSE,
                            _lcCamera.ViewProjectionMatrix);
    }
}


internal void
_lcCameraUpdateProjectionMatrix(lcGenericMessage_t *message)
{
    lcWindowResizeMessage_t *resize;

    resize = (lcWindowResizeMessage_t *)message;

    lcMatrix4CreateOrthographicProjectionMatrix(
        _lcCamera.ProjectionMatrix, -((f32)resize->Width / 2.0f),
        ((f32)resize->Width / 2.0f), -((f32)resize->Height / 2.0f),
        ((f32)resize->Height / 2.0f));

    lcCameraRecalculateViewProjectionMatrix(); 
}

internal void
_lcCameraInit(f32 *position)
{
    u32 windowSize[2];

    _lcCamera.ViewMatrix = malloc(sizeof(f32) * 16);
    _lcCamera.ProjectionMatrix = malloc(sizeof(f32) * 16);
    _lcCamera.ViewProjectionMatrix = malloc(sizeof(f32) * 16);

    _lcCamera.UniformLocation = -1;

    memcpy(_lcCamera.Position, position, sizeof(f32) * 2);

    lcWindowGetSize(windowSize);

    lcMatrix4CreateOrthographicProjectionMatrix(
        _lcCamera.ProjectionMatrix, -((f32)windowSize[0] / 2.0f),
        ((f32)windowSize[0] / 2.0f), -((f32)windowSize[1] / 2.0f),
        ((f32)windowSize[1] / 2.0f));

    lcMatrix4CreateTranslationMatrix(_lcCamera.ViewMatrix, position[0],
                                     position[1]);

    lcMessageBind(LC_MESSAGE_TYPE_WINDOW_RESIZE,
                   _lcCameraUpdateProjectionMatrix);

    lcCameraRecalculateViewProjectionMatrix();
}

void
lcCameraMove(f32 *offset)
{
    lcVector2Add(_lcCamera.Position, _lcCamera.Position, offset);
  
    lcMatrix4CreateTranslationMatrix(_lcCamera.ViewMatrix,
                                     _lcCamera.Position[0],
                                     _lcCamera.Position[1]);
  
    lcCameraRecalculateViewProjectionMatrix();
}

internal void
_lcCameraDestroy(void)
{
    free(_lcCamera.ViewMatrix);
    free(_lcCamera.ProjectionMatrix);
    free(_lcCamera.ViewProjectionMatrix);
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

