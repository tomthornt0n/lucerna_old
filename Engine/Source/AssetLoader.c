/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna

  Author  : Tom Thornton
  Updated : 23 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_ASSETS_FILE_PATH "assets.data"

static void
lcLoadMasterTexture(void)
{
    FILE *assetsFile;
    char c;

    assetsFile = fopen(LC_ASSETS_FILE_PATH, "rb");
    LC_ASSERT(assetsFile, "Could not open assets file: %s", strerror(errno));

    for (c = fgetc(assetsFile);
         !feof(assetsFile);
         c = fgetc(assetsFile))
    {
        if (c == LCAP_ASSET_TYPE_TEXTURE_ATLAS)
        {
            lcapTextureAtlas_t textureHeader;
            size_t pixelsSize;
            uint8_t *pixels;
            GLuint textureID;

            ungetc(c, assetsFile);

            fread(&textureHeader, sizeof(lcapTextureAtlas_t), 1, assetsFile);

            pixelsSize = textureHeader.Width *
                         textureHeader.Width *
                         4;

            pixels = malloc(pixelsSize);
            fread(pixels, pixelsSize, 1, assetsFile);

            gl.GenTextures(1, &textureID);
            gl.BindTexture(GL_TEXTURE_2D, textureID);

            gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
            gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            gl.TexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA8,
                         textureHeader.Width,
                         textureHeader.Width,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         pixels);

            free(pixels);

            return;
        }
        else if (c == LCAP_ASSET_TYPE_SPRITE)
        {
            /* skip */
            fseek(assetsFile, sizeof(lcapSprite_t) - 1, SEEK_CUR);
        }
        else
        {
            LC_ASSERT(false,
                      "Could not load master texture: corrupt assets file.");
        }
    }

    LC_ASSERT(false,
              "Could not load master texture: asset not found.");
}

lcGenericAsset_t *
lcLoadAsset(char *name)
{
    FILE *assetsFile;
    char c;

    assetsFile = fopen(LC_ASSETS_FILE_PATH, "rb");
    LC_ASSERT(assetsFile, "Could not open assets file.");

    for (c = fgetc(assetsFile);
         !feof(assetsFile);
         c = fgetc(assetsFile))
    {
        if (c == LCAP_ASSET_TYPE_TEXTURE_ATLAS)
        {
            lcapTextureAtlas_t textureHeader;

            ungetc(c, assetsFile);

            fread(&textureHeader, sizeof(lcapTextureAtlas_t), 1, assetsFile);

            fseek(assetsFile,
                  textureHeader.Width * textureHeader.Width * 4,
                  SEEK_CUR);
        }
        else if (c == LC_ASSET_TYPE_SPRITE)
        {
            lcapSprite_t sprite;

            ungetc(c, assetsFile);
            fread(&sprite, sizeof(sprite), 1, assetsFile);

            if (0 == strcmp(sprite.Name, name))
            {
                lcAssetSprite_t *result;

                result = malloc(sizeof(*result));
                result->Header.Type = LC_ASSET_TYPE_SPRITE;
                memcpy(result->Min, sprite.Min, 2 * sizeof(float));
                memcpy(result->Max, sprite.Max, 2 * sizeof(float));

                return (lcGenericAsset_t *)result;
            }
        }
        else if (c == LC_ASSET_TYPE_SHADER)
        {
            lcapShader_t shader;

            ungetc(c, assetsFile);
            fread(&shader, sizeof(shader), 1, assetsFile);

            if (0 == strcmp(shader.Name, name))
            {
                lcAssetShader_t *result;
                char *vertexSrc;
                char *fragmentSrc;


                result = malloc(sizeof(*result));
                result->Header.Type = LC_ASSET_TYPE_SPRITE;

                vertexSrc = malloc(shader.VertexLength);
                fragmentSrc = malloc(shader.FragmentLength);
                fread(vertexSrc, shader.VertexLength, 1, assetsFile);
                fread(fragmentSrc, shader.FragmentLength, 1, assetsFile);

                result->Shader = lcShaderCreate(vertexSrc, fragmentSrc);

                return (lcGenericAsset_t *)result;
            }
        }
        else
        {
            LC_ASSERT(false, "Could not load asset '%s': corrupt assets file.", name);
        }
    }

    LC_ASSERT(false, "Could not load asset '%s': asset not found.", name);
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

