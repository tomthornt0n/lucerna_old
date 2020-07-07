/* Array Backed List                                                           */
/*                                                                             */
/* works with standard subscript operator by incrementing the returned pointer */
/* so that additional information can be stored before the array itself.       */
/*                                                                             */
/* (uint32_t *)list - 2: number of elements                                    */
/* (uint32_t *)list - 1: max number of elements                                */
/*                                                                             */
/* To avoid excessive heap allocations, when the list becomes full, it is      */
/* reallocated to be double the size.                                          */
/*                                                                             */
/* In addition, when removing elements from the list, the alloacted memory is  */
/* not shrunk untill the number of elements decreases to half of the max number*/
/* of elements.                                                                */

#define LC_LIST_CREATE(list, type) LC_ASSERT((list) == NULL,                     \
                                       "Uninitialised list must be NULL");     \
                             (list) = (type *)                                   \
                             (                                                 \
                                    (uint32_t *)malloc(sizeof(type) +          \
                                    2 * sizeof(uint32_t)) + 2                  \
                             );                                                \
                             *((uint32_t *)(list) - 1) = 1;                      \
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

