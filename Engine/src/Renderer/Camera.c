typedef struct
{
    lcRenderer_t *Renderer;
    char *UniformName;

    float ProjectionMatrix[16];
    float Position[2];
} lcCamera_t;

lc_CreateCamera(lcCamera_t* output,
                lcRenderer_t *renderer,
                char *viewProjectionUniformName,
                uint32_t windowWidth, uint32_t windowHeight,
                float *position)
{
    memcpy(output->Position, position, 2 * sizeof(float));
    output->Renderer = renderer;
    output->UniformName = viewProjectionUniformName;

    lc_CreateOrthographicProjectionMatrix( output->ProjectionMatrix,
                                          -((float) windowWidth / 2.0f),
                                           ((float) windowWidth / 2.0f),
                                          -((float) windowHeight / 2.0f),
                                           ((float) windowHeight / 2.0f),
                                           1.0f,
                                          -1.0f
                                          );    
    float translationMatrix[16];
    lc_CreateTranslationMatrix(translationMatrix,
                               position[0], position[1]);

    float viewProjectionMatrix[16];
    lc_Matrix4Multiply(viewProjectionMatrix,
                       translationMatrix, output->ProjectionMatrix);

    lc_ShaderUploadUniformMatrix4(output->Renderer->Shader,
                                  output->UniformName,
                                  viewProjectionMatrix);
}

lc_MoveCamera(lcCamera_t *camera,
              float *offset)
{

}
