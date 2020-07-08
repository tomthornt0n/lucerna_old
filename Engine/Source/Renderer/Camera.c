struct
{
    char *_UniformName;
    GLint _UniformLocation;

    float *_ProjectionMatrix;
    float *_ViewMatrix;
    float *_ViewProjectionMatrix;

    float Position[2];
} lc_Camera;

void _lc_CameraRecalculateViewProjectionMatrix(void)
{
    lc_Matrix4Multiply(lc_Camera._ViewProjectionMatrix,
                       lc_Camera._ProjectionMatrix,
                       lc_Camera._ViewMatrix);

    glUniformMatrix4fv(lc_Camera._UniformLocation,
                       1, GL_FALSE,
                       lc_Camera._ViewProjectionMatrix);
}


void _lc_CameraUpdateProjectionMatrix(lcMessage_t message)
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

void lc_CameraInit(char *uniformName,
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

void lc_CameraMove(float* offset)
{
    lc_Vector2Add(lc_Camera.Position,
                  lc_Camera.Position, offset);

    lc_Matrix4CreateTranslationMatrix(lc_Camera._ViewMatrix,
                                      lc_Camera.Position[0],
                                      lc_Camera.Position[1]);

    _lc_CameraRecalculateViewProjectionMatrix();
}

void lc_CameraDestroy(void)
{
    free(lc_Camera._ViewMatrix);
    free(lc_Camera._ProjectionMatrix);
    free(lc_Camera._ViewProjectionMatrix);
}
