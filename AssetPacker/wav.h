#ifndef WAV_H
#define WAV_H

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

void wavRead(char *filename,
             int *dataRate,
             int *bitsPerSample,
             int *channels,
             int *dataSize,
             uint8_t *data);

#ifdef __cplusplus
}
#endif

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef WAV_IMPLEMENTATION

#define WAV_RIFF_CODE(a, b, c, d) (((uint32_t)(a) << 0)  |                     \
                                   ((uint32_t)(b) << 8)  |                     \
                                   ((uint32_t)(c) << 16) |                     \
                                   ((uint32_t)(d) << 24))

#pragma pack(push, 1)
typedef struct
{
    uint32_t ID;
    uint32_t Size;
} wavRiffHeader_t;

typedef struct
{
    wavRiffHeader_t Header;
    uint16_t wFormatTag;
    uint16_t nChannels;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint16_t nBlockAlign;
    uint16_t wBitsPerSample;
} wavFmtChunk_t;

typedef struct
{
    wavRiffHeader_t Header;
} wavDataChunk_t;

typedef struct
{
    wavRiffHeader_t Header;
    uint32_t WAVE;
    wavFmtChunk_t Fmt;
    wavDataChunk_t Data;
} wavMasterChunk_t;
#pragma pack(pop)

void
wavRead(char *filename,
        int *dataRate,
        int *bitsPerSample,
        int *channels,
        int *dataSize,
        uint8_t *data)
{
    FILE *f;
    wavMasterChunk_t master;
    int m; /* bytes per sample */

    f = fopen(filename, "rb");
    
    fread(&master, sizeof(master), 1, f);

    m = (int)(master.Fmt.wBitsPerSample) / 8;

    assert(master.Header.ID == WAV_RIFF_CODE('R', 'I', 'F', 'F'));
    assert(master.WAVE == WAV_RIFF_CODE('W', 'A', 'V', 'E'));
    assert(master.Fmt.Header.ID == WAV_RIFF_CODE('f', 'm', 't', ' '));
    assert(master.Fmt.Header.Size == 16);
    assert(master.Fmt.wFormatTag == 1 && "Compressed WAVs not supported!");
    assert(master.Fmt.nBlockAlign == (m * master.Fmt.nChannels));
    assert(master.Fmt.nAvgBytesPerSec == (master.Fmt.nSamplesPerSec * master.Fmt.nBlockAlign));
    assert(master.Data.Header.ID == WAV_RIFF_CODE('d', 'a', 't', 'a'));

    if (dataRate)      { *dataRate = (int)(master.Fmt.nSamplesPerSec); }
    if (bitsPerSample) { *bitsPerSample = (int)(master.Fmt.wBitsPerSample); }
    if (channels)      { *channels = (int)(master.Fmt.nChannels); }
    if (dataSize)      { *dataSize = (int)(master.Data.Header.Size); }
    if (data)          { fread(data, master.Data.Header.Size, 1, f); }

    fclose(f);
}
#endif
#endif 

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

