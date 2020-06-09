/* used to create bitfields */
#define BIT(x) (1 << x)

/* Concatinates count number of strings together
   Returns a heap allocated string of the result
   and transfers ownership of the memory
*/
char* lc_Concatenate(int count, ...)
{
    va_list ap;
    int i;

    // Find required length to store merged string
    int len                                                = 1; // room for null terminator character
    va_start(ap, count);
    for (i = 0; i < count; i++)
        len                                               += strlen(va_arg(ap, char*));
    va_end(ap);

    // Allocate memory to concat strings
    char* merged                                           = calloc(len, sizeof(char));
    int null_pos                                           = 0;

    // Actually concatenate strings
    va_start(ap, count);
    for (i = 0; i < count; i++)
    {
        char* s                                            = va_arg(ap, char*);
        strcpy(merged + null_pos, s);
        null_pos                                          += strlen(s);
    }
    va_end(ap);

    return merged;
}

/* Loads a file and allocates memory for it
   Returns the length of the file(excluding null terminator)
   or -1 if loading the file failed
*/
int lc_LoadFile(const char* filename, char** result)
{
    int size                                               = 0;
    FILE* f                                                = fopen(filename, "rb");
    if (f == NULL)
    {
        *result                                            = NULL;
        return -1;
    }
    fseek(f, 0, SEEK_END);
    size                                                   = ftell(f);
    fseek(f, 0, SEEK_SET);
    *result                                                = malloc(size + 1);
    if (size != fread(*result, sizeof(char), size, f))
    {
        free(*result);
        return -1;
    }
    fclose(f);
    (*result)[size]                                        = 0;
    return size;
}
