/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 27 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void
lcVector2Add(f32 *result,
             f32 *vector1, f32 *vector2)
{
    result[0] = vector1[0] + vector2[0];
    result[1] = vector1[1] + vector2[1];
}

void
lcVector2Subtract(f32 *result,
                  f32 *vector1, f32 *vector2)
{
    result[0] = vector1[0] - vector2[0];
    result[1] = vector1[1] - vector2[1];
}

void
lcVector2Multiply(f32 *result,
                  f32 *vector1, f32 *vector2)
{
    result[0] = vector1[0] * vector2[0];
    result[1] = vector1[1] * vector2[1];
}

void
lcVector2Divide(f32 *result,
                f32 *vector1, f32 *vector2)
{
    result[0] = vector1[0] / vector2[0];
    result[1] = vector1[1] / vector2[1];
}

f32
lcVector2Dot(f32 *vector1, f32 *vector2)
{
    return ((vector1[0] * vector2[0]) +
            (vector1[1] * vector2[1]));
}

void
lcVector3Add(f32 *result,
             f32 *vector1, f32 *vector2)
{
    result[0] = vector1[0] + vector2[0];
    result[1] = vector1[1] + vector2[1];
    result[2] = vector1[2] + vector2[2];
}

void
lcVector3Subtract(f32 *result,
                  f32 *vector1, f32 *vector2)
{
    result[0] = vector1[0] - vector2[0];
    result[1] = vector1[1] - vector2[1];
    result[2] = vector1[2] - vector2[2];
}

void
lcVector3Multiply(f32 *result,
                  f32 *vector1, f32 *vector2)
{
    result[0] = vector1[0] * vector2[0];
    result[1] = vector1[1] * vector2[1];
    result[2] = vector1[2] * vector2[2];
}

void
lcVector3Divide(f32 *result,
                f32 *vector1, f32 *vector2)
{
    result[0] = vector1[0] / vector2[0];
    result[1] = vector1[1] / vector2[1];
    result[2] = vector1[2] / vector2[2];
}

f32
lcVector3Dot(f32 *vector1, f32 *vector2)
{
    return ((vector1[0] * vector2[0]) +
            (vector1[1] * vector2[1]) +
            (vector1[2] * vector2[2]));
}

void
lcVector4Add(f32 *result,
             f32 *vector1, f32 *vector2)
{
    __m128 _left, _right, _answer;

    _left   = _mm_load_ps(vector1);
    _right  = _mm_load_ps(vector2);
    _answer = _mm_add_ps(_left, _right);

    _mm_store_ps(result, _answer);
}

void
lcVector4Subtract(f32 *result,
                  f32 *vector1, f32 *vector2)
{
    __m128 _left, _right, _answer;

    _left   = _mm_load_ps(vector1);
    _right  = _mm_load_ps(vector2);
    _answer = _mm_sub_ps(_left, _right);

    _mm_store_ps(result, _answer);
}

void
lcVector4Multiply(f32 *result,
                  f32 *vector1, f32 *vector2)
{
    __m128 _left, _right, _result;

    _left   = _mm_load_ps(vector1);
    _right  = _mm_load_ps(vector2);
    _result = _mm_mul_ps(_left, _right);
    
    _mm_store_ps(result, _result);
}

void
lcVector4Divide(f32 *result,
                f32 *vector1, f32 *vector2)
{
    __m128 _left, _right, _answer;

    _left   = _mm_load_ps(vector1);
    _right  = _mm_load_ps(vector2);
    _answer = _mm_div_ps(_left, _right);

    _mm_store_ps(result, _answer);
}

f32
lcVector4Dot(f32 *vector1, f32 *vector2)
{
    return ((vector1[0] * vector2[0]) +
            (vector1[1] * vector2[1]) +
            (vector1[2] * vector2[2]) +
            (vector1[3] * vector2[3]));
}

void
lcMatrix4Multiply(f32 *result,
                  f32 *matrix1, f32 *matrix2)
{
    const __m128 _matrix2Column1 = _mm_load_ps(&(matrix1[0]));
    const __m128 _matrix2Column2 = _mm_load_ps(&(matrix1[4]));
    const __m128 _matrix2Column3 = _mm_load_ps(&(matrix1[8]));
    const __m128 _matrix2Column4 = _mm_load_ps(&(matrix1[12]));

    f32 *leftRowPointer = &matrix2[0];
    f32 *resultRowPointer = &result[0];

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


void
lcMatrix4CreateOrthographicProjectionMatrix(f32 *matrix,
                                            f32 left, f32 right,
                                            f32 top, f32 bottom)
{
    memset(matrix, 0, 16 * sizeof(f32));
    matrix[0]  = 2.0f / (right - left);
    matrix[5]  = 2.0f / (top - bottom);
    matrix[10] = -1.0f;
    matrix[12] = -(right + left) / (right - left);
    matrix[13] = -(top + bottom) / (top - bottom);
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;
}

void
lcMatrix4CreateTranslationMatrix(f32 *matrix,
                                 f32 x, f32 y)
{
    memset(matrix, 0, 16 * sizeof(f32));

    matrix[0]  = 1.0f;
    matrix[5]  = 1.0f;
    matrix[10] = 1.0f;
    matrix[12] = x;
    matrix[13] = y;
    matrix[15] = 1.0f;
}

void
lcMatrix4CreateScaleMatrix(f32 *matrix,
                           f32 x, f32 y)
{
    memset(matrix, 0, 16 * sizeof(f32));

    matrix[0]  = x;
    matrix[5]  = y;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;
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

