struct
{
    char *_UniformName;

    float _ProjectionMatrix[16];
    float _ViewMatrix[16];

    /* 1 if projection or view matrix has changed since the view-projection
       matrix was last calculated and uploaded,
       Otherwise, 0
    */
    uint8_t _Changed;

    float Position[2];
} lc_Camera;

void _lc_CameraUpdateProjectionMatrix(lcMessage_t message)
{
    LC_CORE_LOG_DEBUG("Window size changed. Recalculating projection matrix");

    lc_Matrix4CreateOrthographicProjectionMatrix(
         lc_Camera._ProjectionMatrix,
        -((float) message.WindowResize.Width / 2.0f),
         ((float) message.WindowResize.Width / 2.0f),
        -((float) message.WindowResize.Height / 2.0f),
         ((float) message.WindowResize.Height / 2.0f),
         1.0f, -1.0f
    );

    lc_Camera._Changed = 1;
}

void lc_CameraInit(char *uniformName,
                   float *position)
{
    memcpy(lc_Camera.Position,
           position, sizeof(float) * 2);

    lc_Camera._UniformName = uniformName;

    lc_Matrix4CreateOrthographicProjectionMatrix(
         lc_Camera._ProjectionMatrix,
        -((float) lc_Window.Width / 2.0f),
         ((float) lc_Window.Width / 2.0f),
        -((float) lc_Window.Height / 2.0f),
         ((float) lc_Window.Height / 2.0f),
         1.0f, -1.0f
    );

    lc_Matrix4CreateTranslationMatrix(lc_Camera._ViewMatrix,
                                      position[0], position[1]);
    lc_MessageBind(LC_MESSAGE_TYPE_WINDOW_RESIZE,
                   _lc_CameraUpdateProjectionMatrix);

    lc_Camera._Changed = 1;
}

void lc_CameraMove(float* offset)
{
    lc_Vector2Add(lc_Camera.Position,
                  lc_Camera.Position, offset);

    lc_Matrix4CreateTranslationMatrix(lc_Camera._ViewMatrix,
                                      lc_Camera.Position[0],
                                      lc_Camera.Position[1]);

    lc_Camera._Changed = 1;
}
