/****************************************************************************/
/* Small general purpose utility functions that have no place anywhere else */
/****************************************************************************/


/* used to create bitfields */
#define BIT(x) (1 << x)

#define _LC_STRINGIFY(x) #x
#define _LC_TO_STRING(x) _LC_STRINGIFY(x)
#define __LINE_STRING__ _LC_TO_STRING(__LINE__)

/* Concatinates count number of strings together
   Returns a heap allocated string of the result
*/
uint8_t *lc_Concatenate(int count, ...)
{
    va_list ap;
    int i;

    int len = 1;
    va_start(ap, count);
    for (i = 0; i < count; i++)
        len += strlen(va_arg(ap, uint8_t*));
    va_end(ap);

    uint8_t *merged = calloc(len, sizeof(uint8_t));
    int null_pos = 0;

    va_start(ap, count);
    for (i = 0; i < count; i++)
    {
        uint8_t *s = va_arg(ap, uint8_t*);
        strcpy(merged + null_pos, s);
        null_pos += strlen(s);
    }
    va_end(ap);

    return merged;
}

