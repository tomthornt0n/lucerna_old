/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 05 August 2020
  License : MIT, at end of file
  Notes   : PNG decoder based on https://handmadehero.org/
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* NOTE(tbt): Loads a file and allocates memory for it
   Returns the length of the file(excluding null terminator)
   or -1 if loading the file failed
*/
int
lcLoaderReadFile(const char *filename,
                 uint8_t **result)
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


/* NOTE(tbt): stop the compiler introducing padding, so these structs match
   exactly to what is in the file
*/
#pragma pack(push, 1)

typedef struct
{
    uint8_t Signature[8];
} lcPNGHeader_t;
uint8_t lcPNGSignature[] = { 0x89, 0x50, 0x4e, 0x47, 0xd, 0xa, 0x1a, 0xa };

typedef struct
{
    uint32_t Length;
    char Type[4];
} lcPNGChunkHeader_t;

typedef struct
{
    uint32_t Width;
    uint32_t Height;
    uint8_t BitDepth;
    uint8_t ColourType;
    uint8_t CompressionMethod;
    uint8_t FilterMethod;
    uint8_t InterlaceMethod;

} lcPNGChunkDataIHDR_t;

typedef struct
{
    uint8_t ZLibMethodFlags;
    uint8_t AdditionalFlags;
} lcPNGChunkDataIDATHeader_t;

typedef struct
{
    uint8_t CheckValue;
} lcPNGChunkDataIDATFooter_t;

typedef struct
{
    uint32_t CRC;
} lcPNGChunkFooter_t;

#pragma pack(pop)

typedef struct
{
    uint16_t Symbol;
    uint16_t BitsUsed;
} lcPNGHuffmanEntry_t;

#define LC_PNG_HUFFMAN_MAX_BIT_COUNT 16
typedef struct
{
    uint32_t MaxCodeLengthInBits;
    uint32_t EntryCount;
    lcPNGHuffmanEntry_t *Entries;
} lcPNGHuffman_t;

typedef struct
{
    uint32_t ContentsSize;
    uint8_t *Contents;

    uint32_t BitBuffer;
    uint32_t BitCount;
} lcPNGBitStream_t;

static void
lcPNGEndianSwap(uint32_t *value)
{
    uint32_t v = *value;

    *value = (v << 24) |
             ((v & 0xFF00) << 8) |
             ((v >> 8) & 0xFF00) |
             (v >> 24);
}

static uint32_t
lcPNGPeekBits(lcPNGBitStream_t *buffer,
              uint8_t bitCount)
{
    LC_ASSERT(bitCount <= 32, "bitCount cannot be greater than 32!"); 

    uint32_t result = 0;

    while ((buffer->BitCount < bitCount) &&
           (buffer->ContentsSize))
    {
        uint8_t byte = *buffer->Contents;
        ++buffer->Contents;
        --buffer->ContentsSize;
        buffer->BitBuffer |= (uint32_t)byte << buffer->BitCount;
        buffer->BitCount += 8;
    }

    result = buffer->BitBuffer & ((1 << bitCount) - 1);

    return result;
}

static void
lcPNGDiscardBits(lcPNGBitStream_t *buffer,
                 uint8_t bitCount)
{
    buffer->BitCount -= bitCount;
    buffer->BitBuffer >>= bitCount;
}

static uint32_t
lcPNGConsumeBits(lcPNGBitStream_t *buffer,
                 uint8_t bitCount)
{
    uint32_t result = lcPNGPeekBits(buffer, bitCount);
    lcPNGDiscardBits(buffer, bitCount);
    return result;
}

static lcPNGHuffman_t
lcPNGHuffmanCreate(uint32_t maxCodeLengthInBits)
{
    LC_ASSERT(maxCodeLengthInBits <= LC_PNG_HUFFMAN_MAX_BIT_COUNT,
              "Huffman code length exceeds maximum");

    lcPNGHuffman_t result;
    result.MaxCodeLengthInBits = maxCodeLengthInBits;
    result.EntryCount = 1 << maxCodeLengthInBits;
    result.Entries = malloc(result.EntryCount * sizeof(lcPNGHuffmanEntry_t));

    return result;
}

static void
lcPNGComputeHuffman(uint32_t symbolCount,
                    uint32_t *symbolCodeLength,
                    uint32_t offset,
                    lcPNGHuffman_t *result)
{
    LC_CORE_LOG_DEBUG("lcPNGComputeHuffman");
    int i, j;

    uint32_t codeLengthHist[LC_PNG_HUFFMAN_MAX_BIT_COUNT] = {0};
    uint32_t nextUnusedCode[LC_PNG_HUFFMAN_MAX_BIT_COUNT];

    for (i = 0; i < symbolCount; ++i)
    {
        uint32_t count = symbolCodeLength[i];
        LC_ASSERT(count < LC_PNG_HUFFMAN_MAX_BIT_COUNT,
                  "Corrupt PNG");
        ++codeLengthHist[count];
    }

    codeLengthHist[0] = 0;
    nextUnusedCode[0] = 0;

    for (i = 0; i < LC_PNG_HUFFMAN_MAX_BIT_COUNT; ++i)
    {
        nextUnusedCode[i] = (nextUnusedCode[i - 1] +
                             codeLengthHist[i - 1])
                            << 1;
    }

    for (i = 0; i < symbolCount; ++i)
    {
        uint32_t codeLengthInBits = symbolCodeLength[i];
        if (!codeLengthInBits) continue;

        LC_ASSERT(codeLengthInBits < LC_ARRAY_COUNT(nextUnusedCode),
                  "Error computing PNG huffman.");

        uint32_t code = nextUnusedCode[codeLengthInBits]++;

        uint32_t entryCount = 1 << (result->MaxCodeLengthInBits -
                                    codeLengthInBits);

        for(j = 0; j < entryCount; ++j)
        {
            /* uint32_t index = (j << codeLengthInBits) | code; */
            uint32_t index = 0;
            int k;
            for (k = 0; k <= result->MaxCodeLengthInBits / 2; ++k)
            {
                uint32_t inv = (result->MaxCodeLengthInBits - (k + 1));
                uint32_t baseIndex = (j << codeLengthInBits) | code;
                index |= ((baseIndex >> k) & 0x1) << inv;
                index |= ((baseIndex >> inv) & 0x1) << k;
            }

            lcPNGHuffmanEntry_t *entry = result->Entries + index;
            entry->BitsUsed = (uint16_t)codeLengthInBits;
            entry->Symbol = (uint16_t)(i + offset);
        }
    }
}

static uint32_t
lcPNGHuffmanDecode(lcPNGHuffman_t *huffman,
                   lcPNGBitStream_t *input)
{
    uint32_t entryIndex = lcPNGPeekBits(input,
                                        huffman->MaxCodeLengthInBits);

    lcPNGHuffmanEntry_t entry = huffman->Entries[entryIndex];
    uint32_t result = entry.Symbol;
    lcPNGDiscardBits(input, entry.BitsUsed);
    LC_ASSERT(entry.BitsUsed, "Error decoding PNG huffman");

    return result;
}

/* NOTE(tbt): Very minimal png loading.
   Not intended to be robust or general purpose in any way
*/
lcImage_t lcLoaderParsePNG(char *filename)
{
    lcImage_t result;

    uint8_t *fileData;
    int fileSize = lcLoaderReadFile(filename, &fileData);

    lcPNGBitStream_t idatData;
    idatData.Contents = NULL;
    idatData.ContentsSize = 0;
    idatData.BitCount = 0;
    idatData.BitBuffer = 0;
    
    LC_ASSERT(fileSize != -1, "Could not load file %s", filename);
    LC_CORE_LOG_INFO("Loading PNG: %s", filename);

    uint8_t *at = fileData;

    lcPNGHeader_t *header = (lcPNGHeader_t *)at;
    at += sizeof(lcPNGHeader_t);

    LC_ASSERT(!memcmp(lcPNGSignature, header->Signature, 8),
              "  Not a PNG file!");

    while (1)
    {
        lcPNGChunkHeader_t *chunkHeader = (lcPNGChunkHeader_t *)at;
        at += sizeof(lcPNGChunkHeader_t);
        void *chunkData = at;
        lcPNGEndianSwap(&chunkHeader->Length);
        at += chunkHeader->Length;
        lcPNGChunkFooter_t *chunkFooter = (lcPNGChunkFooter_t *)at;
        lcPNGEndianSwap(&chunkFooter->CRC);
        at += sizeof(lcPNGChunkFooter_t);

        LC_CORE_LOG_INFO("  %.4s:", chunkHeader->Type);

        if (!memcmp(chunkHeader->Type, "IHDR", 4))
        {
            lcPNGChunkDataIHDR_t *ihdr = chunkData;
            
            lcPNGEndianSwap(&ihdr->Width);
            lcPNGEndianSwap(&ihdr->Height);

            LC_CORE_LOG_INFO("    Width: %u", ihdr->Width);
            LC_CORE_LOG_INFO("    Height: %u", ihdr->Height);
            LC_CORE_LOG_INFO("    Bit Depth: %u", ihdr->BitDepth);
            LC_CORE_LOG_INFO("    Colour Type: %u", ihdr->ColourType);
            LC_CORE_LOG_INFO("    Compression Method: %u", ihdr->CompressionMethod);
            LC_CORE_LOG_INFO("    Filter Method: %u", ihdr->FilterMethod);
            LC_CORE_LOG_INFO("    Interlace Method: %u", ihdr->InterlaceMethod);

            result.Width = ihdr->Width;
            result.Height = ihdr->Height;

            LC_ASSERT(ihdr->BitDepth == 8          &&
                      ihdr->ColourType == 6        &&
                      ihdr->CompressionMethod == 0 &&
                      ihdr->FilterMethod == 0      &&
                      ihdr->InterlaceMethod == 0,
                      "Unsupported PNG!");

        }
        else if (!memcmp(chunkHeader->Type, "IDAT", 4))
        {
            LC_CORE_LOG_INFO("    Concatenating...");
            idatData.Contents = realloc(idatData.Contents,
                                        idatData.ContentsSize +
                                        chunkHeader->Length);

            memcpy(idatData.Contents + idatData.ContentsSize,
                   chunkData, chunkHeader->Length);

            idatData.ContentsSize += chunkHeader->Length;
        }
        else if (!memcmp(chunkHeader->Type, "IEND", 4))
        {
            LC_CORE_LOG_INFO("    Parsing IDAT data...");
            uint8_t *startOfIDATData = idatData.Contents; /* NOTE(tbt): keep a
                                                             pointer to the
                                                             start of the
                                                             IDAT data so it
                                                             can be freed after
                                                             idatData.Contents
                                                             has been
                                                             incremented
                                                          */

            uint8_t cm     = lcPNGConsumeBits(&idatData, 4);
            uint8_t cInfo  = lcPNGConsumeBits(&idatData, 4);
            uint8_t fCheck = lcPNGConsumeBits(&idatData, 5);
            uint8_t fDict  = lcPNGConsumeBits(&idatData, 1);
            uint8_t fLevel = lcPNGConsumeBits(&idatData, 2);

            LC_CORE_LOG_INFO("      ZLIB header:");
            LC_CORE_LOG_INFO("        cm: %u", cm);
            LC_CORE_LOG_INFO("        cInfo: %u", cInfo);
            LC_CORE_LOG_INFO("        fCheck: %u", fCheck);
            LC_CORE_LOG_INFO("        fDict: %u", fDict);
            LC_CORE_LOG_INFO("        fLevel: %u", fLevel);

            LC_ASSERT(cm == 8 &&
                      fDict == 0,
                      "    Invalid ZLIB header");

            /* NOTE(tbt): assume that it is a valid image after checking the
               PNG and zlib headers, so now allocate memory for the decompressed
               data
            */
            result.Data = malloc(result.Width *
                                 result.Height *
                                 4); /* NOTE(tbt): bytes per pixel */

            uint32_t bfinal = 0;
            while (bfinal == 0)
            {
                bfinal = lcPNGConsumeBits(&idatData, 1);
                uint32_t btype = lcPNGConsumeBits(&idatData, 2);

                LC_ASSERT(btype == 0 ||
                          btype == 2,
                          "Unrecognised zlib block type");

                if (btype == 0)
                {
                    idatData.BitBuffer = 0;
                    idatData.BitCount = 0;
                    uint32_t len = lcPNGConsumeBits(&idatData, 16);
                    uint32_t nlen = lcPNGConsumeBits(&idatData, 16);

                    LC_CORE_LOG_INFO("      ZLIB block:");
                    LC_CORE_LOG_INFO("        bfinal: %u", bfinal);
                    LC_CORE_LOG_INFO("        btype: 0");
                    LC_CORE_LOG_INFO("        len: %u", len);
                    LC_CORE_LOG_INFO("        nlen: %u", nlen);
                }
                else
                {
                    lcPNGHuffman_t litLenHuffman = lcPNGHuffmanCreate(15);
                    lcPNGHuffman_t distanceHuffman = lcPNGHuffmanCreate(15);

                    uint32_t hlit = lcPNGConsumeBits(&idatData, 5);
                    uint32_t hdist = lcPNGConsumeBits(&idatData, 5);
                    uint32_t hclen = lcPNGConsumeBits(&idatData, 4);

                    hlit += 257;
                    hdist += 1;
                    hclen += 4;

                    LC_CORE_LOG_INFO("      ZLIB block:");
                    LC_CORE_LOG_INFO("        bfinal: %u", bfinal);
                    LC_CORE_LOG_INFO("        btype: 2");
                    LC_CORE_LOG_INFO("        hlit: %u", hlit);
                    LC_CORE_LOG_INFO("        hdist: %u", hdist);
                    LC_CORE_LOG_INFO("        hclen: %u", hclen);

                    uint32_t hclenSwizzle[] =
                    {
                        16, 17, 18, 0, 8, 7, 9, 6, 10, 5,
                        11, 4, 12, 3, 13, 2, 14, 1, 15
                    };
                    
                    uint32_t hclenTable[LC_ARRAY_COUNT(hclenSwizzle)] = {0};

                    int i;

                    for (i = 0; i < hclen; ++i)
                    {
                        hclenTable[hclenSwizzle[i]] =
                            lcPNGConsumeBits(&idatData, 3);
                    }

                    lcPNGHuffman_t dictionaryHuffman = lcPNGHuffmanCreate(7);
                    lcPNGComputeHuffman(LC_ARRAY_COUNT(hclenSwizzle),
                                        hclenTable,
                                        0,
                                        &dictionaryHuffman);

                    uint32_t litLenDistTable[512];
                    uint32_t litLenCount = 0;

                    while (litLenCount < (hlit + hdist))
                    {
                        uint32_t encodedLength =
                            lcPNGHuffmanDecode(&dictionaryHuffman,
                                               &idatData);
                        uint32_t repeatCount = 1;
                        uint32_t repeatValue = 0;

                        if (encodedLength <= 15)
                        {
                            repeatValue = encodedLength;
                        }
                        else if (encodedLength == 16)
                        {
                            repeatCount = lcPNGConsumeBits(&idatData, 2) + 3;
                            repeatValue = litLenDistTable[litLenCount - 1];
                        }
                        else if (encodedLength == 17)
                        {
                            repeatCount = lcPNGConsumeBits(&idatData, 3) + 3;
                        }
                        else if (encodedLength == 18)
                        {
                            repeatCount = lcPNGConsumeBits(&idatData, 7) + 7;
                        }
                        else
                        {
                            LC_ASSERT(0,
                                      "Unexpected LITLEN: %u",
                                      encodedLength);
                        }

                        while (repeatCount--)
                        {
                            litLenDistTable[litLenCount++] = repeatValue;
                        }
                    }

                    lcPNGComputeHuffman(hlit,
                                          litLenDistTable,
                                          0,
                                          &litLenHuffman);

                    lcPNGComputeHuffman(hdist,
                                          litLenDistTable + hlit,
                                          0,
                                          &distanceHuffman);

                    while (1)
                    {
                        uint32_t litLen = lcPNGHuffmanDecode(&litLenHuffman,
                                                               &idatData);
                        litLen = 256; /* NOTE(tbt): for debug purposes */

                        if (litLen < 256)
                        {
                            uint32_t out = litLen;
                            /* TODO(tbt): write to output buffer */
                        }
                        else if (litLen > 256)
                        {
                            uint32_t length = litLen - 256;
                            uint32_t distance =
                                lcPNGHuffmanDecode(&distanceHuffman,
                                                     &idatData);

                            int i;
                            for (i = 0; i < litLen; ++i)
                            {
                                /* TODO(tbt): write to output buffer */
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
            
            LC_CORE_LOG_INFO("    Reaching end of PNG file");
            free(startOfIDATData);
            break;
        }
        else
        {
            LC_CORE_LOG_WARN("    Skipping ancillary chunk");
        }
    }

    free(fileData);
    return result;
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

