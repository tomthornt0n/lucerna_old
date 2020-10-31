#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(unix) || defined(__unix__) || defined(__unix)
#include <dirent.h>
#endif

#include "lcap.h"

#define LCAP_TEXTURES_DIRECTORY "../Client/Assets/Textures/"
#define LCAP_SHADERS_DIRECTORY "../Client/Assets/Shaders/"
#define LCAP_AUDIO_DIRECTORY "../Client/Assets/Audio/"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WAV_IMPLEMENTATION
#include "wav.h"

/* All of this is awful. I'll make it better at some point */

typedef struct
{
    char Name[LCAP_NAME_MAX_LEN];

    uint8_t *Pixels;
    uint32_t Size;

    float Min[2];
    float Max[2];
} texture_t;

typedef struct
{
    char Name[LCAP_NAME_MAX_LEN];
    char *VertexSource;
    char *FragmentSource;
} shader_t;

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

void
GetAllFilesInDir(char *path,
                 int *count,
                 char ***list)
{
#ifdef _WIN32
    WIN32_FIND_DATA dir;
    HANDLE d = INVALID_HANDLE_VALUE;

    *count = 0;
    *list = NULL;

    size_t lengthOfArg = strlen(path);
    if (lengthOfArg > (MAX_PATH - 2))
    {
        fprintf(stderr,
                "ERROR: path is too long.");
        exit(-1);
    }

    char fullpath[MAX_PATH];
    memcpy(fullpath, path, MAX_PATH);
    strcat(fullpath, "*");

    d = FindFirstFile(fullpath, &dir);

    do
    {
        if (!(dir.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            *list = realloc(*list, ++(*count) * sizeof(**list));
            int size = strlen(dir.cFileName) + 1;
            (*list)[(*count) - 1] = malloc(size);
            memcpy((*list)[(*count) - 1], dir.cFileName, size);
        }
    } while (FindNextFile(d, &dir) != 0);

    FindClose(d);
#else
    *count = 0;
    *list = NULL;

    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_REG)
            {
                *list = realloc(*list, ++(*count) * sizeof(**list));
                int size = strlen(dir->d_name) + 1;
                (*list)[(*count) - 1] = malloc(size);
                memcpy((*list)[(*count) - 1], dir->d_name, size);
            }
        }
    closedir(d);
  }
#endif
}

void
FreeDirectoryList(int count,
                  char **list)
{
    int textureIndex;
    for (textureIndex = 0;
         textureIndex < count;
         ++textureIndex)
    {
        free(list[textureIndex]);
    }
    free(list);
}

void
ReadEntireFile(char *path,
         char **result)
{
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    (*result) = malloc(size + 1);
    fread((*result), 1, size, f);
    fclose(f);

    (*result)[size] = 0;
}

void
PackTextures(FILE *out)
{
    int textureIndex;

    int textureCount;
    char **texturePaths;

    GetAllFilesInDir(LCAP_TEXTURES_DIRECTORY,
                     &textureCount,
                     &texturePaths);

    texture_t *textures = malloc(textureCount * sizeof(*textures));

    for (textureIndex = 0;
         textureIndex < textureCount;
         ++textureIndex)
    {
        int charIndex;
        for (charIndex = 0;
             charIndex < (LCAP_NAME_MAX_LEN - 1) &&
             texturePaths[textureIndex][charIndex] != '.';
             textures[textureIndex].Name[++charIndex] = 0)
        {
            textures[textureIndex].Name[charIndex] =
                texturePaths[textureIndex][charIndex];
        }

        char *fullpath = malloc(strlen(texturePaths[textureIndex]) +
                                strlen(LCAP_TEXTURES_DIRECTORY) + 
                                1);
        memcpy(fullpath,
               LCAP_TEXTURES_DIRECTORY,
               strlen(LCAP_TEXTURES_DIRECTORY) + 1);
        strcat(fullpath, texturePaths[textureIndex]);

        int width, height, components;
        textures[textureIndex].Pixels = stbi_load(fullpath,
                                                  &width, &height,
                                                  &components, 4);
        if (textures[textureCount].Pixels == NULL)
        {
            fprintf(stderr,
                    "ERROR: could not load texture '%s'.\n",
                    fullpath);
            exit(-1);
        }
        else if (width != height)
        {
            fprintf(stderr,
                    "ERROR: textures must be square. "
                    "Got width of %d and height of %d for texture '%s'.\n",
                    width,
                    height,
                    fullpath);
            exit(-1);
        }
        else if ((width & (width - 1)) != 0)
        {
            fprintf(stderr,
                    "ERROR: the size of a texture must be a power of two. "
                    "Got width of: %d for texture '%s'.\n",
                    width,
                    fullpath);
            exit(-1);
        }


        textures[textureIndex].Size = width;
    }

    Sort(textures, 0, textureCount);

    int totalTextureArea = 0;
    for (textureIndex = 0; textureIndex < textureCount; ++textureIndex)
    {
        totalTextureArea += textures[textureIndex].Size *
                            textures[textureIndex].Size;
    }

    int binArea;
    for (binArea = 4; binArea < totalTextureArea; binArea *= 4)
      ;
    int binWidth = sqrt(binArea);
    uint8_t *bin = malloc(binArea * 4);
    uint8_t *placedMask = malloc(binArea);
    memset(bin, 0, binArea * 4);
    memset(placedMask, 0, binArea);

    for (textureIndex = textureCount - 1; textureIndex >= 0; --textureIndex)
    {
        int j, x, y;
        for (j = 0; j < binArea; ++j) {
          if (!(placedMask[j])) {
            break;
          }
        }

        x = j % binWidth;
        y = j / binWidth;

        WriteTexture(bin, placedMask, binWidth, &textures[textureIndex], x, y);
        textures[textureIndex].Min[0] = (float)x / (float)binWidth;
        textures[textureIndex].Min[1] = (float)y / (float)binWidth;
        textures[textureIndex].Max[0] = (float)(x + textures[textureIndex].Size) / (float)binWidth;
        textures[textureIndex].Max[1] = (float)(y + textures[textureIndex].Size) / (float)binWidth;
    }

    lcapChunkHeader_t atlasHeader;
    atlasHeader.Type = LCAP_ASSET_TYPE_TEXTURE_ATLAS;
    atlasHeader.Size = binArea * 4;

    fwrite(&atlasHeader, sizeof(atlasHeader), 1, out);
    fwrite(bin, 4, binArea, out);
    for (textureIndex = 0; textureIndex < textureCount; ++textureIndex)
    {
        lcapChunkHeader_t header;
        lcapSprite_t sprite;

        header.Type = LCAP_ASSET_TYPE_SPRITE;
        header.Size = sizeof(lcapSprite_t);
        memcpy(sprite.Name, textures[textureIndex].Name, LCAP_NAME_MAX_LEN);
        memcpy(sprite.Min, textures[textureIndex].Min, sizeof(float) * 2);
        memcpy(sprite.Max, textures[textureIndex].Max, sizeof(float) * 2);

        fwrite(&header, sizeof(header), 1, out);
        fwrite(&sprite, sizeof(sprite), 1, out);
    }

    FreeDirectoryList(textureCount, texturePaths);
}

void
PackShaders(FILE *out)
{
    int sourceIndex;

    int sourceCount;
    char **shaderPaths;

    GetAllFilesInDir(LCAP_SHADERS_DIRECTORY,
                     &sourceCount,
                     &shaderPaths);

    int shaderCount = 0;
    shader_t *shaders = NULL;

    for (sourceIndex = 0;
         sourceIndex < sourceCount;
         ++sourceIndex)
    {
        char name[LCAP_NAME_MAX_LEN];
        int charIndex;
        for (charIndex = 0;
             charIndex < (LCAP_NAME_MAX_LEN - 1) &&
             shaderPaths[sourceIndex][charIndex] != '.';
             name[++charIndex] = 0)
        {
            name[charIndex] = shaderPaths[sourceIndex][charIndex];
        }

        char *fullpath = malloc(strlen(shaderPaths[sourceIndex]) +
                                strlen(LCAP_SHADERS_DIRECTORY) +
                                1);
        memcpy(fullpath,
               LCAP_SHADERS_DIRECTORY,
               strlen(LCAP_TEXTURES_DIRECTORY) + 1);
        strcat(fullpath, shaderPaths[sourceIndex]);

        if (strstr(shaderPaths[sourceIndex], ".frag"))
        {
            bool found = false;

            int i;
            for (i = 0; i < shaderCount; ++i)
            {
                if (0 == strcmp(name, shaders[i].Name))
                {
                    if (shaders[i].FragmentSource == NULL)
                    {
                        ReadEntireFile(fullpath, &(shaders[i].FragmentSource));
                        found = true;
                    }
                    else
                    {
                        fprintf(stderr,
                                "ERROR: duplicate shader name '%s'.",
                                name);
                    }
                }
            }

            if (!found)
            {
                shaders = realloc(shaders, sizeof(*shaders) * ++shaderCount);
                int i = shaderCount - 1;

                memcpy(shaders[i].Name, name, LCAP_NAME_MAX_LEN);

                shaders[i].VertexSource = NULL;
                ReadEntireFile(fullpath, &(shaders[i].FragmentSource));
            }
        }
        else if (strstr(shaderPaths[sourceIndex], ".vert"))
        {
            bool found = false;

            int i;
            for (i = 0; i < shaderCount; ++i)
            {
                if (0 == strcmp(name, shaders[i].Name))
                {
                    if (shaders[i].VertexSource == NULL)
                    {
                        ReadEntireFile(fullpath, &(shaders[i].VertexSource));
                        found = true;
                    }
                    else
                    {
                        fprintf(stderr,
                                "ERROR: duplicate shader name '%s'.",
                                name);
                    }
                }
            }

            if (!found)
            {
                shaders = realloc(shaders, sizeof(*shaders) * ++shaderCount);
                int i = shaderCount - 1;

                memcpy(shaders[i].Name, name, LCAP_NAME_MAX_LEN);

                shaders[i].FragmentSource = NULL;
                ReadEntireFile(fullpath, &(shaders[i].VertexSource));
            }
        }
        else
        {
            fprintf(stderr,
                    "ERROR: unrecognised extension for file '%s'.\n",
                    shaderPaths[sourceIndex]);
            exit(-1);
        }
    }

    int shaderIndex;
    for (shaderIndex = 0;
         shaderIndex < shaderCount;
         ++shaderIndex)
    {
        if (shaders[shaderIndex].VertexSource == NULL ||
            shaders[shaderIndex].FragmentSource == NULL)
        {
            fprintf(stderr,
                    "ERROR: missing source for shader '%s'.\n",
                    shaders[shaderIndex].Name);
            exit(-1);
        }

        lcapChunkHeader_t header;
        lcapShader_t shader;

        memcpy(shader.Name, shaders[shaderIndex].Name, LCAP_NAME_MAX_LEN);
        shader.VertexLength = strlen(shaders[shaderIndex].VertexSource) + 1;
        shader.FragmentLength = strlen(shaders[shaderIndex].FragmentSource) + 1;

        header.Type = LCAP_ASSET_TYPE_SHADER;
        header.Size = sizeof(lcapShader_t) +
                      shader.VertexLength +
                      shader.FragmentLength;

        fwrite(&header, sizeof(header), 1, out);
        fwrite(&shader, sizeof(shader), 1, out);
        fwrite(shaders[shaderIndex].VertexSource,
               shader.VertexLength,
               1,
               out);
        fwrite(shaders[shaderIndex].FragmentSource,
               shader.FragmentLength,
               1,
               out);

        free(shaders[shaderIndex].VertexSource);
        free(shaders[shaderIndex].FragmentSource);
    }
}

void
PackSounds(FILE *out)
{
    int soundIndex;
    int soundCount;
    char **soundPaths;

    GetAllFilesInDir(LCAP_AUDIO_DIRECTORY,
                     &soundCount,
                     &soundPaths);

    for (soundIndex = 0;
         soundIndex < soundCount;
         ++soundIndex)
    {
        /* TODO(tbt): convert all audio to same format */
        char *fullpath;
        int dataSize;
        unsigned char *data;

        lcapChunkHeader_t header;
        lcapSound_t sound;

        fullpath = malloc(strlen(soundPaths[soundIndex]) +
                          strlen(LCAP_AUDIO_DIRECTORY) + 
                          1);

        memcpy(fullpath,
               LCAP_AUDIO_DIRECTORY,
               strlen(LCAP_AUDIO_DIRECTORY) + 1);
        strcat(fullpath, soundPaths[soundIndex]);

        wavRead(fullpath,
                NULL,
                NULL,
                NULL,
                &dataSize,
                NULL);
        data = malloc(dataSize);
        wavRead(fullpath,
                NULL,
                NULL,
                NULL,
                NULL,
                data);

        header.Type = LCAP_ASSET_TYPE_SOUND;
        header.Size = sizeof(lcapSound_t) +
                      dataSize;


        int charIndex;
        for (charIndex = 0;
             charIndex < (LCAP_NAME_MAX_LEN - 1) &&
             soundPaths[soundIndex][charIndex] != '.';
             sound.Name[++charIndex] = 0)
        {
            sound.Name[charIndex] = soundPaths[soundIndex][charIndex];
        }

        fwrite(&header, sizeof(header), 1, out);
        fwrite(&sound, sizeof(sound), 1, out);
        fwrite(data, dataSize, 1, out);
    }
}

int
main(int argc,
     char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <output directory>\n", argv[0]);
    }

    FILE *out = fopen(argv[1], "wb");
    if (out == NULL)
    {
        fprintf(stderr,
                "ERROR: Could not '%s' for writing.\n",
                argv[1]);
        exit(-1);
    }

    PackTextures(out);
    PackShaders(out);
    PackSounds(out);

    fclose(out);
}

