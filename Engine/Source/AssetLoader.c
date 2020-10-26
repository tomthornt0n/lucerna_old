/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna

  Author  : Tom Thornton
  Updated : 26 Oct 2020
  License : MIT, at end of file
  Notes   : Not very good.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "Lucerna.h"
#define LC_ASSETS_FILE_PATH "assets.data"

static void
lcLoadMasterTexture(void)
{
    FILE *assetsFile;
    lcapChunkHeader_t header;
    GLuint textureID;
    uint32_t width;
    uint8_t *pixels;

    assetsFile = fopen(LC_ASSETS_FILE_PATH, "rb");
    LC_ASSERT(assetsFile, "Could not open assets file: %s", strerror(errno));
    
    for (fread(&header, sizeof(header), 1, assetsFile);
         header.Type != LCAP_ASSET_TYPE_TEXTURE_ATLAS ||
         feof(assetsFile);
         fread(&header, sizeof(header), 1, assetsFile));

    LC_ASSERT(header.Type == LCAP_ASSET_TYPE_TEXTURE_ATLAS,
              "Could not find texture atlas!");

    width = (uint32_t)sqrt((double)(header.Size / 4));
    pixels = malloc(header.Size);
    fread(pixels, header.Size, 1, assetsFile);

    gl.GenTextures(1, &textureID);
    gl.BindTexture(GL_TEXTURE_2D, textureID);

    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    gl.TexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 width,
                 width,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 pixels);

    free(pixels);
    fclose(assetsFile);
}

void *
lcLoadAsset(char *name,
            int type)
{
    FILE *assetsFile;
    lcapChunkHeader_t header;

    assetsFile = fopen(LC_ASSETS_FILE_PATH, "rb");
    LC_ASSERT(assetsFile, "Could not open assets file.");

    while (!feof(assetsFile))
    {
        fread(&header, sizeof(header), 1, assetsFile);

        if (header.Type != type)
        {
            fseek(assetsFile, header.Size, SEEK_CUR);
            continue;
        }

        if (type == LC_ASSET_TYPE_SHADER)
        {
            lcapShader_t shader;

            fread(&shader,
                  sizeof(shader),
                  1,
                  assetsFile);

            if (0 == strcmp(shader.Name, name))
            {
                lcShader_t *result;
                char *vertexSrc;
                char *fragmentSrc;

                result = malloc(sizeof(*result));

                vertexSrc = malloc(shader.VertexLength);
                fragmentSrc = malloc(shader.FragmentLength);
                fread(vertexSrc, shader.VertexLength, 1, assetsFile);
                fread(fragmentSrc, shader.FragmentLength, 1, assetsFile);

                *result = lcShaderCreate(vertexSrc, fragmentSrc);

                fclose(assetsFile);
                return result;
            }
            else
            {
                fseek(assetsFile,
                      shader.VertexLength + shader.FragmentLength,
                      SEEK_CUR);
            }
        }
        else if (type == LC_ASSET_TYPE_SOUND)
        { 
            lcapSound_t sound;

            fread(&sound,
                  sizeof(sound),
                  1,
                  assetsFile);

            if (0 == strcmp(sound.Name, name))
            {
                int dataSize = header.Size - sizeof(sound);
                lcAudioSource_t *result;

                result = malloc(sizeof(*result));
                result->BufferSize = dataSize;
                result->Buffer = malloc(dataSize);
                fread(result->Buffer, dataSize, 1, assetsFile);
                result->Playhead = 0;
                result->State = LC_AUDIO_PAUSED;

                return result;
            }
        }
        else if (type == LC_ASSET_TYPE_SPRITE)
        { 
            lcapSprite_t sprite;

            fread(&sprite,
                  sizeof(sprite),
                  1,
                  assetsFile);

            if (0 == strcmp(sprite.Name, name))
            {
                lcSprite_t *result;

                result = malloc(sizeof(*result));
                memcpy(result->Min, sprite.Min, 2 * sizeof(float));
                memcpy(result->Max, sprite.Max, 2 * sizeof(float));

                fclose(assetsFile);
                return result;
            }
        }
        else
        {
            LC_ASSERT(false,
                      "Failure loading asset '%s'. Invalid assets file!",
                      name);
        }
    }

    LC_ASSERT(false,
              "Failure loading asset '%s'. Could not find asset!",
              name);
    fclose(assetsFile);
    return NULL;
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

