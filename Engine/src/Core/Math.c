void lc_Vector4Add(float* result,
                   float* vector1, float* vector2)
{
    __m128 _left, _right, _answer;

    _left   = _mm_load_ps(vector1);
    _right  = _mm_load_ps(vector2);
    _answer = _left + _right;

    _mm_store_ps(result, _answer);
}

void lc_Vector4Subtract(float* result,
                   float* vector1, float* vector2)
{
    __m128 _left, _right, _answer;

    _left   = _mm_load_ps(vector1);
    _right  = _mm_load_ps(vector2);
    _answer = _left - _right;

    _mm_store_ps(result, _answer);
}

void lc_Vector4Multiply(float* result,
                   float* vector1, float* vector2)
{
    __m128 _left, _right, _result;

    _left   = _mm_load_ps(vector1);
    _right  = _mm_load_ps(vector2);
    _result = _left * _right;
    
    _mm_store_ps(result, _result);
}

void lc_Vector4Divide(float* result,
                   float* vector1, float* vector2)
{
    __m128 _left, _right, _answer;

    _left   = _mm_load_ps(vector1);
    _right  = _mm_load_ps(vector2);
    _answer = _left / _right;

    _mm_store_ps(result, _answer);
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

        __m128 _x = _matrix1Row1 * _matrix2Column1;
        __m128 _y = _matrix1Row2 * _matrix2Column2;
        __m128 _z = _matrix1Row3 * _matrix2Column3;
        __m128 _w = _matrix1Row4 * _matrix2Column4;

        __m128 _result = _x + _y + _z + _w;
        _mm_store_ps(resultRowPointer, _result);
    }
}

void lc_CreateOrthographicProjectionMatrix(float *matrix,
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

void lc_CreateTranslationMatrix(float *matrix,
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

void lc_CreateScaleMatrix(float *matrix,
                          float xScale, float yScale)
{
    memset(matrix, 0, 16 * sizeof(float));

    matrix[0]  = xScale;
    matrix[5]  = yScale;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;
}
