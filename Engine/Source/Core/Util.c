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

typedef struct
{
    uint32_t width, height;
    uint32_t bitDepth;
    uint8_t *data;
} lcImage_t;

/* work in progress */
void lc_DecodePNG(uint8_t *inputData, int inputLength,
                  lcImage_t* output)
{

    /* check file signature */
    if ( inputData[0] != 0x89 ||
         inputData[1] != 0x50 ||
         inputData[2] != 0x4e ||
         inputData[3] != 0x47 ||
         inputData[4] != 0x0d ||
         inputData[5] != 0x0a ||
         inputData[6] != 0x1a ||
         inputData[7] != 0x0a )
    {
        LC_ASSERT(0, "Not valid PNG!");
    }

    int compressedImageDataSize = 0;
    uint8_t* compressedImageData = NULL;
    
    long pos = 8;
    while (1)
    {
        if (inputData[pos + 4] == 'I' &&
            inputData[pos + 5] == 'H' &&
            inputData[pos + 6] == 'D' &&
            inputData[pos + 7] == 'R' )
        {
            LC_CORE_LOG_DEBUG("Reading \x1b[32mIHDR \x1b[0mchunk");

            int i;
            for (i = 0; i < 4; ++i)
            {
                output->width  = output->width  << 8 | inputData[pos +  8 + i];
                output->height = output->height << 8 | inputData[pos + 12 + i];
            }

            output->bitDepth = inputData[pos + 16];
            LC_ASSERT((output->bitDepth == 8) || (output->bitDepth == 16),
                      "Only 8 or 16 bit depths are supported for colour mode "
                      "'true colour with alpha'!");

            LC_ASSERT(inputData[pos + 17] == 6,
                      "Currently only RGBA images are supported.")

            LC_ASSERT(inputData[pos + 20] == 0,
                      "Interlaced images are not currently supported.")

            pos += 25;
        }

        else if (inputData[pos + 4] == 'I' &&
                 inputData[pos + 5] == 'D' &&
                 inputData[pos + 6] == 'A' &&
                 inputData[pos + 7] == 'T' )
        {
            /* image data */
            LC_CORE_LOG_DEBUG("Reading \x1b[32mIDAT \x1b[0mchunk");

            uint32_t chunkDataSize;

            int i;
            for (i = 0; i < 4; ++i)
            {
                chunkDataSize = chunkDataSize  << 8 | inputData[pos + i];
            }

            compressedImageData = realloc(compressedImageData,
                                          compressedImageDataSize +
                                          chunkDataSize);

            memcpy(&(compressedImageData[compressedImageDataSize]),
                   &(inputData[pos + 8]), chunkDataSize);

            compressedImageDataSize = chunkDataSize;
            pos += (chunkDataSize + 12);
        }

        else if (inputData[pos + 4] == 'I' &&
                 inputData[pos + 5] == 'E' &&
                 inputData[pos + 6] == 'N' &&
                 inputData[pos + 7] == 'D' )
        {
            /* end of file */
            LC_CORE_LOG_DEBUG("Reading \x1b[32mIEND \x1b[0mchunk");
            return;
        }

        else
        {
            LC_CORE_LOG_DEBUG("\x1b[34mSkipping ancillary chunk %.4s",
                        &(inputData[pos + 4]));

            uint32_t chunkDataSize;

            int i;
            for (i = 0; i < 4; ++i)
            {
                chunkDataSize = chunkDataSize << 8 | inputData[pos + i];
            }

            pos += (chunkDataSize + 12);
        }
    }
}
