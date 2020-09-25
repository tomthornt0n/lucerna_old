/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 24 August 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

struct
{
    char *UniformName;
    GLint UniformLocation;

    float *ProjectionMatrix;
    float *ViewMatrix;
    float *ViewProjectionMatrix;

    float Position[2];
} lcCamera;

static void
lcCameraRecalculateViewProjectionMatrix(void)
{
    lcMatrix4Multiply(lcCamera.ViewProjectionMatrix,
                      lcCamera.ProjectionMatrix,
                      lcCamera.ViewMatrix);

    gl.UniformMatrix4fv(lcCamera.UniformLocation,
                       1, GL_FALSE,
                       lcCamera.ViewProjectionMatrix);
}


static void
lcCameraUpdateProjectionMatrix(lcGenericMessage_t *message)
{
    lcWindowResizeMessage_t *resize = (lcWindowResizeMessage_t *)message;

    lcMatrix4CreateOrthographicProjectionMatrix(
         lcCamera.ProjectionMatrix,
        -((float) resize->Width / 2.0f),
         ((float) resize->Width / 2.0f),
        -((float) resize->Height / 2.0f),
         ((float) resize->Height / 2.0f)
    );

   lcCameraRecalculateViewProjectionMatrix(); 
}

void
lcCameraInit(char *uniformName,
             float *position)
{
    lcCamera.ViewMatrix = malloc(sizeof(float) * 16);
    lcCamera.ProjectionMatrix = malloc(sizeof(float) * 16);
    lcCamera.ViewProjectionMatrix = malloc(sizeof(float) * 16);

    memcpy(lcCamera.Position,
           position, sizeof(float) * 2);

    lcCamera.UniformName = uniformName;

    uint32_t windowSize[2];
    lcWindowGetSize(windowSize);

    lcMatrix4CreateOrthographicProjectionMatrix(
         lcCamera.ProjectionMatrix,
        -((float)windowSize[0]  / 2.0f),
         ((float)windowSize[0]  / 2.0f),
        -((float)windowSize[1] / 2.0f),
         ((float)windowSize[1] / 2.0f)
    );

    lcMatrix4CreateTranslationMatrix(lcCamera.ViewMatrix,
                                      position[0], position[1]);

    lcMessageBind(LC_MESSAGE_TYPE_WINDOW_RESIZE,
                   lcCameraUpdateProjectionMatrix);

    lcCameraRecalculateViewProjectionMatrix();
}

void
lcCameraMove(float* offset)
{
    lcVector2Add(lcCamera.Position,
                  lcCamera.Position, offset);

    lcMatrix4CreateTranslationMatrix(lcCamera.ViewMatrix,
                                      lcCamera.Position[0],
                                      lcCamera.Position[1]);

    lcCameraRecalculateViewProjectionMatrix();
}

void
lcCameraDestroy(void)
{
    free(lcCamera.ViewMatrix);
    free(lcCamera.ProjectionMatrix);
    free(lcCamera.ViewProjectionMatrix);
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

