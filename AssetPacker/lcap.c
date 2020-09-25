#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "lcap.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef struct
{
    char Name[LCAP_NAME_MAX_LEN];

    uint8_t *Pixels;
    uint32_t Size;

    float Min[2];
    float Max[2];
} texture_t;

static char
fpeekc(FILE *file)
{
    char c = getc(file);
    return c == EOF ? EOF : ungetc(c, file);
}

static void
EatWhiteSpace(FILE *file)
{
    for (;isspace(fpeekc(file));fgetc(file));
}

static void
Swap(texture_t *a,
     texture_t *b)
{
    texture_t t = *a;
    *a = *b;
    *b = t;
}

static uint32_t
Partition(texture_t *array,
          int low,
          int high)
{
    texture_t pivot = array[high];
    int i = low - 1;

    int j;
    for (j = low; j <= (high - 1); ++j)
    {
        if (array[j].Size < pivot.Size)
        {
            ++i;
            Swap(array + i, array + j);
        }
    }
    Swap(&array[i + 1], &array[high]);
    return (i + 1);
}

static void
Sort(texture_t *array,
     int low,
     int high)
{
    if (low < high)
    {
        uint32_t p = Partition(array, low, high);

        Sort(array, low, p - 1);
        Sort(array, p + 1, high);
    }
}

static void
WriteTexture(uint8_t*bin,
             uint8_t *mask,
             uint32_t binWidth,
             texture_t *texture,
             uint32_t xOffset,
             uint32_t yOffset)
{
    int i;
    for (i = 0; i < texture->Size; ++i)
    {
        memcpy(bin + ((((i + yOffset) * binWidth) + xOffset) * 4),
               texture->Pixels + (i * texture->Size * 4),
               texture->Size * 4);
        memset(mask + (((i + yOffset) * binWidth) + xOffset),
               0xff,
               texture->Size);
    }
}

int
main(int argc,
     char **argv)
{
    int i;

    int textureCount = 0;
    texture_t *textures = NULL;

    /* parse file paths and load images */    
    char *outputPath = NULL;

    int argrumentIndex;
    bool expectingOutputPath;
    for (argrumentIndex = 1;
         argrumentIndex < argc;
         ++argrumentIndex)
    {

        if (0 == strcmp("-o", argv[argrumentIndex]))
        {
            expectingOutputPath = true;
        }
        else if (expectingOutputPath)
        {
            outputPath = argv[argrumentIndex];
            expectingOutputPath = false;
        }
        else
        {
            /* assume textures (only asset type currently supported) */
            /* TODO: parse file extension to determine asset type */
            FILE *textureList = fopen(argv[argrumentIndex], "r");
            char c;
            while (fpeekc(textureList) != EOF)
            {
                int nameLen = 0;
                char name[LCAP_NAME_MAX_LEN];
                while ((c = fgetc(textureList)) != ':' &&
                       nameLen < (LCAP_NAME_MAX_LEN - 1))
                {
                        name[nameLen++] = c;
                }
                if (nameLen >= LCAP_NAME_MAX_LEN)
                {
                    fprintf(stderr,
                            "\x1b[31m"
                            "ERROR: name longer than %d characters."
                            "\x1b[0m\n",
                            (LCAP_NAME_MAX_LEN - 1));
                    exit(-1);
                }
                name[nameLen] = 0;

                EatWhiteSpace(textureList);

                int pathLen = 0;
                char *path = malloc(1);
                while ((c = fgetc(textureList)) != '\n')
                {
                    path[pathLen++] = c;
                    path = realloc(path, pathLen + 1);
                }
                path[pathLen] = 0;

                int width, height, components;

                textures = realloc(textures, sizeof *textures * (textureCount + 1));

                memcpy(textures[textureCount].Name, name, LCAP_NAME_MAX_LEN);
                textures[textureCount].Pixels = stbi_load(path,
                                                          &width,
                                                          &height,
                                                          &components,
                                                          4);

                if (textures[textureCount].Pixels == NULL)
                {
                    fprintf(stderr,
                            "\x1b[31m"
                            "ERROR: could not load texture '%s'."
                            "\x1b[0m\n",
                            path);
                    exit(-1);
                }
                else if (width != height)
                {
                    fprintf(stderr,
                            "\x1b[31m"
                            "ERROR: textures must be square. "
                            "Got width of %d and height of %d for texture '%s'."
                            "\x1b[0m\n",
                            width,
                            height,
                            path);
                    exit(-1);
                }
                else if ((width & (width - 1)) != 0)
                {
                    fprintf(stderr,
                            "\x1b[31m"
                            "ERROR: the size of a texture must be a power of two. "
                            "Got width of: %d for texture '%s'."
                            "\x1b[0m\n",
                            width,
                            path);
                    exit(-1);
                }

                textures[textureCount].Size = width;

                ++textureCount;

                EatWhiteSpace(textureList);
            }
        }
    }

    if (outputPath == NULL)
    {
        fprintf(stderr,
                "\x1b[31m"
                "Expected option '-o': no output path specified."
                "\x1b[0m\n");
        exit(-1);
    }

    /* sort textures by size */
    Sort(textures, 0, textureCount - 1);

    /* find total area */
    int area = 0;
    for (i = 0; i < textureCount; ++i)
    {
        area += textures[i].Size * textures[i].Size;
    }

    /* find area of smallest power of two square that is greater than the total
       area of the textures
    */
    int binArea;
    for (binArea = 4; binArea < area; binArea *= 4);
    int binWidth = sqrt(binArea);
    uint8_t *bin = malloc(binArea * 4);
    uint8_t *placedMask = malloc(binArea);
    memset(bin, 0, binArea * 4);
    memset(placedMask, 0, binArea);

    /* pack textures into bin */
    for (i = textureCount - 1; i >= 0; --i)
    {
        int j, x, y;
        for (j = 0; j < binArea; ++j)
        {
            if (!(placedMask[j]))
            {
                break;
            }
        }

        x = j % binWidth;
        y = j / binWidth;

        WriteTexture(bin, placedMask, binWidth, &textures[i], x, y);
        textures[i].Min[0] = (float)x / (float)binWidth;
        textures[i].Min[1] = (float)y / (float)binWidth;
        textures[i].Max[0] = (float)(x + textures[i].Size) / (float)binWidth;
        textures[i].Max[1] = (float)(y + textures[i].Size) / (float)binWidth;
    }

    FILE *out = fopen(outputPath, "wb");
    if (out == NULL)
    {
        fprintf(stderr,
                "\x1b[31m"
                "ERROR: Could not open output file for writing."
                "\x1b[0m\n");
        exit(-1);
    }

    lcapTextureAtlas_t masterTexture =
    {
        .Type = LCAP_ASSET_TYPE_TEXTURE_ATLAS,
        .Width = binWidth
    };


    fwrite(&masterTexture, sizeof(lcapTextureAtlas_t), 1, out);
    fwrite(bin, 4, binArea, out);
    for (i = 0; i < textureCount; ++i)
    {
        lcapSprite_t sprite = { .Type = LCAP_ASSET_TYPE_SPRITE };
        memcpy(sprite.Name, textures[i].Name, LCAP_NAME_MAX_LEN);
        memcpy(sprite.Min, textures[i].Min, sizeof(float) * 2);
        memcpy(sprite.Max, textures[i].Max, sizeof(float) * 2);
        fwrite(&sprite, sizeof(lcapSprite_t), 1, out);
    }
    fclose(out);
}

