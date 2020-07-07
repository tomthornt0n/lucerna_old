void lc_Vector2Add(float *result,
                   float *vector1, float *vector2)
{
    result[0] = vector1[0] + vector2[0];
    result[1] = vector1[1] + vector2[1];
}

void lc_Vector2Subtract(float *result,
                        float *vector1, float *vector2)
{
    result[0] = vector1[0] - vector2[0];
    result[1] = vector1[1] - vector2[1];
}

void lc_Vector2Multiply(float *result,
                        float *vector1, float *vector2)
{
    result[0] = vector1[0] * vector2[0];
    result[1] = vector1[1] * vector2[1];
}

void lc_Vector2Divide(float *result,
                      float *vector1, float *vector2)
{
    result[0] = vector1[0] / vector2[0];
    result[1] = vector1[1] / vector2[1];
}

float lc_Vector2Dot(float *vector1, float *vector2)
{
    return ((vector1[0] * vector2[0]) +
            (vector1[1] * vector2[1]));
}

void lc_Vector3Add(float *result,
                   float *vector1, float *vector2)
{
    result[0] = vector1[0] + vector2[0];
    result[1] = vector1[1] + vector2[1];
    result[2] = vector1[2] + vector2[2];
}

void lc_Vector3Subtract(float *result,
                        float *vector1, float *vector2)
{
    result[0] = vector1[0] - vector2[0];
    result[1] = vector1[1] - vector2[1];
    result[2] = vector1[2] - vector2[2];
}

void lc_Vector3Multiply(float *result,
                        float *vector1, float *vector2)
{
    result[0] = vector1[0] * vector2[0];
    result[1] = vector1[1] * vector2[1];
    result[2] = vector1[2] * vector2[2];
}

void lc_Vector3Divide(float *result,
                      float *vector1, float *vector2)
{
    result[0] = vector1[0] / vector2[0];
    result[1] = vector1[1] / vector2[1];
    result[2] = vector1[2] / vector2[2];
}

float lc_Vector3Dot(float *vector1, float *vector2)
{
    return ((vector1[0] * vector2[0]) +
            (vector1[1] * vector2[1]) +
            (vector1[2] * vector2[2]));
}

void lc_Vector4Add(float *result,
                   float *vector1, float *vector2)
{
    __m128 _left, _right, _answer;

    _left   = _mm_load_ps(vector1);
    _right  = _mm_load_ps(vector2);
    _answer = _mm_add_ps(_left, _right);

    _mm_store_ps(result, _answer);
}

void lc_Vector4Subtract(float *result,
                   float *vector1, float *vector2)
{
    __m128 _left, _right, _answer;

    _left   = _mm_load_ps(vector1);
    _right  = _mm_load_ps(vector2);
    _answer = _mm_sub_ps(_left, _right);

    _mm_store_ps(result, _answer);
}

void lc_Vector4Multiply(float *result,
                   float *vector1, float *vector2)
{
    __m128 _left, _right, _result;

    _left   = _mm_load_ps(vector1);
    _right  = _mm_load_ps(vector2);
    _result = _mm_mul_ps(_left, _right);
    
    _mm_store_ps(result, _result);
}

void lc_Vector4Divide(float *result,
                   float *vector1, float *vector2)
{
    __m128 _left, _right, _answer;

    _left   = _mm_load_ps(vector1);
    _right  = _mm_load_ps(vector2);
    _answer = _mm_div_ps(_left, _right);

    _mm_store_ps(result, _answer);
}

float lc_Vector4Dot(float *vector1, float *vector2)
{
    return ((vector1[0] * vector2[0]) +
            (vector1[1] * vector2[1]) +
            (vector1[2] * vector2[2]) +
            (vector1[3] * vector2[3]));
}

void lc_Matrix4Multiply(float *result,
                        float *matrix1, float *matrix2)
{
    const __m128 _matrix2Column1 = _mm_load_ps(&(matrix1[0]));
    const __m128 _matrix2Column2 = _mm_load_ps(&(matrix1[4]));
    const __m128 _matrix2Column3 = _mm_load_ps(&(matrix1[8]));
    const __m128 _matrix2Column4 = _mm_load_ps(&(matrix1[12]));

    float *leftRowPointer = &matrix2[0];
    float *resultRowPointer = &result[0];

    int i;
    for (i = 0; i < 4; ++i, leftRowPointer += 4, resultRowPointer += 4)
    {
        __m128 _matrix1Row1 = _mm_set1_ps(leftRowPointer[0]);
        __m128 _matrix1Row2 = _mm_set1_ps(leftRowPointer[1]);
        __m128 _matrix1Row3 = _mm_set1_ps(leftRowPointer[2]);
        __m128 _matrix1Row4 = _mm_set1_ps(leftRowPointer[3]);

        __m128 _x = _mm_mul_ps(_matrix1Row1, _matrix2Column1);
        __m128 _y = _mm_mul_ps(_matrix1Row2, _matrix2Column2);
        __m128 _z = _mm_mul_ps(_matrix1Row3, _matrix2Column3);
        __m128 _w = _mm_mul_ps(_matrix1Row4, _matrix2Column4);

        __m128 _result = _mm_add_ps(_mm_add_ps(_x, _y), _mm_add_ps(_z, _w));
        _mm_store_ps(resultRowPointer, _result);
    }
}


void lc_Matrix4CreateOrthographicProjectionMatrix(float *matrix,
                                                  float left, float right,
                                                  float top, float bottom,
                                                  float near, float far)
{
    memset(matrix, 0, 16 * sizeof(float));
    matrix[0]  = 2.0f / (right - left);
    matrix[5]  = 2.0f / (top - bottom);
    matrix[10] = 2.0f / (far - near);
    matrix[12] = -(right + left) / (right - left);
    matrix[13] = -(top + bottom) / (top - bottom);
    matrix[14] = -(far + near) / (far - near);
    matrix[15] = 1.0f;
}

void lc_Matrix4CreateTranslationMatrix(float *matrix,
                                       float xOffset, float yOffset)
{
    memset(matrix, 0, 16 * sizeof(float));

    matrix[0]  = 1.0f;
    matrix[5]  = 1.0f;
    matrix[10] = 1.0f;
    matrix[12] = xOffset;
    matrix[13] = yOffset;
    matrix[15] = 1.0f;
}

void lc_Matrix4CreateScaleMatrix(float *matrix,
                                 float xScale, float yScale)
{
    memset(matrix, 0, 16 * sizeof(float));

    matrix[0]  = xScale;
    matrix[5]  = yScale;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;
}
