/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 25 July 2020
  License : MIT, at end of file
  Notes   : Array Backed List.
            Works with standard subscript operator by incrementing the returned
            pointer so that additional information can be stored before the
            array itself.       
                                                                                        
            (uint32_t *)list - 2: number of elements                                    
            (uint32_t *)list - 1: max number of elements                                
                                                                                        
            To avoid excessive heap allocations, when the list becomes full,
            it is reallocated to be double the size.                                          
                                                                                        
            In addition, when removing elements from the list, the alloacted
            memory is not shrunk untill the number of elements decreases to
            half of the max number of elements.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_LIST_CREATE(list, type) LC_ASSERT((list) == NULL,                   \
                                       "Uninitialised list must be NULL");     \
                             (list) = (type *)                                 \
                             (                                                 \
                                    (uint32_t *)malloc(sizeof(type) +          \
                                    2 * sizeof(uint32_t)) + 2                  \
                             );                                                \
                             *((uint32_t *)(list) - 1) = 1;                    \
                             *((uint32_t *)(list) - 2) = 0

#define LC_LIST_LEN(list) *((uint32_t *)(list) - 2)

#define LC_LIST_PUSH_BACK(list, type, item)                                    \
if (*((uint32_t *)(list) - 2) ==                                               \
    *((uint32_t *)(list) - 1))                                                 \
{                                                                              \
    list = (type *)                                                            \
    (                                                                          \
        (uint32_t *)                                                           \
        (                                                                      \
            realloc((uint32_t *)(list) - 2,                                    \
                   *((uint32_t *)(list) - 1) * 2 * sizeof(type)                \
                   + 2 * sizeof(uint32_t)                                      \
                   )                                                           \
        ) + 2                                                                  \
    );                                                                         \
    *((uint32_t *)(list) - 1) *= 2;                                            \
}                                                                              \
memcpy((list) + *((uint32_t *)(list) - 2),                                     \
       item, sizeof(type));                                                    \
(*((uint32_t *)(list) - 2))++

#define LC_LIST_REMOVE(list, type, index)                                      \
memcpy((list) + index,                                                         \
       (list) + index + 1, (*((uint32_t *)(list) - 1) - (index + 1))           \
                         * sizeof(type));                                      \
(*((uint32_t *)(list) - 2))--;                                                 \
                                                                               \
if (*((uint32_t *)(list) - 2) ==                                               \
    *((uint32_t *)(list) - 1) / 2)                                             \
{                                                                              \
    list = (type *)                                                            \
    (                                                                          \
        (uint32_t *)                                                           \
        (                                                                      \
            realloc((uint32_t *)(list) - 2,                                    \
                   *((uint32_t *)(list) - 1)                                   \
                   / 2                                                         \
                   * sizeof(type)                                              \
                   + 2 * sizeof(uint32_t)                                      \
                   )                                                           \
        ) + 2                                                                  \
    );                                                                         \
    *((uint32_t *)(list) - 1) /= 2;                                            \
}                                                                              \

#define LC_LIST_DESTROY(list) LC_ASSERT((list) != NULL,                        \
                                        "List is already NULL");               \
                           free((uint32_t *)(list) - 2);                       \
                           list = NULL


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

