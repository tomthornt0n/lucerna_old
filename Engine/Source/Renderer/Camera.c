/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 25 July 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

struct
{
    char *_UniformName;
    GLint _UniformLocation;

    float *_ProjectionMatrix;
    float *_ViewMatrix;
    float *_ViewProjectionMatrix;

    float Position[2];
} lc_Camera;

void
_lc_CameraRecalculateViewProjectionMatrix(void)
{
    lc_Matrix4Multiply(lc_Camera._ViewProjectionMatrix,
                       lc_Camera._ProjectionMatrix,
                       lc_Camera._ViewMatrix);

    glUniformMatrix4fv(lc_Camera._UniformLocation,
                       1, GL_FALSE,
                       lc_Camera._ViewProjectionMatrix);
}


void
_lc_CameraUpdateProjectionMatrix(lcMessage_t message)
{
    LC_CORE_LOG_DEBUG("Window size changed. Recalculating projection matrix");

    lc_Matrix4CreateOrthographicProjectionMatrix(
         lc_Camera._ProjectionMatrix,
        -((float) message.WindowResize.Width / 2.0f),
         ((float) message.WindowResize.Width / 2.0f),
        -((float) message.WindowResize.Height / 2.0f),
         ((float) message.WindowResize.Height / 2.0f)
    );

   _lc_CameraRecalculateViewProjectionMatrix(); 
}

void
lc_CameraInit(char *uniformName,
              float *position)
{
    lc_Camera._ViewMatrix = malloc(sizeof(float) * 16);
    lc_Camera._ProjectionMatrix = malloc(sizeof(float) * 16);
    lc_Camera._ViewProjectionMatrix = malloc(sizeof(float) * 16);

    memcpy(lc_Camera.Position,
           position, sizeof(float) * 2);

    lc_Camera._UniformName = uniformName;

    lc_Matrix4CreateOrthographicProjectionMatrix(
         lc_Camera._ProjectionMatrix,
        -((float) lc_Window.Width / 2.0f),
         ((float) lc_Window.Width / 2.0f),
        -((float) lc_Window.Height / 2.0f),
         ((float) lc_Window.Height / 2.0f)
    );

    lc_Matrix4CreateTranslationMatrix(lc_Camera._ViewMatrix,
                                      position[0], position[1]);

    lc_MessageBind(LC_MESSAGE_TYPE_WINDOW_RESIZE,
                   _lc_CameraUpdateProjectionMatrix);

    _lc_CameraRecalculateViewProjectionMatrix();
}

void
lc_CameraMove(float* offset)
{
    lc_Vector2Add(lc_Camera.Position,
                  lc_Camera.Position, offset);

    lc_Matrix4CreateTranslationMatrix(lc_Camera._ViewMatrix,
                                      lc_Camera.Position[0],
                                      lc_Camera.Position[1]);

    _lc_CameraRecalculateViewProjectionMatrix();
}

void
lc_CameraDestroy(void)
{
    free(lc_Camera._ViewMatrix);
    free(lc_Camera._ProjectionMatrix);
    free(lc_Camera._ViewProjectionMatrix);
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

