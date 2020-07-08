/****************************************************************************/
/* Small general purpose utility functions that have no place anywhere else */
/****************************************************************************/


/* used to create bitfields */
#define BIT(x) (1 << x)

/* Concatinates count number of strings together
   Returns a heap allocated string of the result
*/
char *lc_Concatenate(int count, ...)
{
    va_list ap;
    int i;

    int len = 1;
    va_start(ap, count);
    for (i = 0; i < count; i++)
        len += strlen(va_arg(ap, char*));
    va_end(ap);

    char *merged = calloc(len, sizeof(char));
    int null_pos = 0;

    va_start(ap, count);
    for (i = 0; i < count; i++)
    {
        char *s = va_arg(ap, char*);
        strcpy(merged + null_pos, s);
        null_pos += strlen(s);
    }
    va_end(ap);

    return merged;
}

