/* Loads a file and allocates memory for it
   Returns the length of the file(excluding null terminator)
   or -1 if loading the file failed
*/
int lc_LoadFile(const char *filename, uint8_t **result)
{
    int size = 0;
    FILE *f = fopen(filename, "rb");
    if (f == NULL)
    {
        *result = NULL;
        return -1;
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    *result = malloc(size + 1);
    if (size != fread(*result, sizeof(uint8_t), size, f))
    {
        free(*result);
        return -1;
    }
    fclose(f);
    (*result)[size] = 0;
    return size;
}
