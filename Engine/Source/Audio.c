/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna

  Author  : Tom Thornton
  Updated : 30 Oct 2020
  License : MIT, at end of file
  Notes   : Just to get something playing.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_AUDIO_SOURCE_HAS_FLAG(source, flag) (source->Flags & flag)

/* clamp a between x and y */
#define LC_AUDIO_CLAMP(a, x, y) (a < x ? x : a > y ? y : a)

enum lcAudioState
{
    LC_AUDIO_STATE_PLAYING,
    LC_AUDIO_STATE_PAUSED
};

enum lcAudioSourceFlags
{
    LC_SOURCE_FLAG_NO_FLAGS = 0,

    LC_SOURCE_FLAG_ACTIVE  = 1 << 0, /* Source is in _lcAudio.Playing? */
    LC_SOURCE_FLAG_REWIND  = 1 << 1, /* Reset Playhead at next playback? */
    LC_SOURCE_FLAG_LOOPING = 1 << 2  /* Continue playing after reaching end? */
};

struct _lcAudioSource_t
{
    u8 *Stream;               /* PCM audio data */
    lcAudioSource_t *Next;    /* Next source in list */
    u32 Playhead;             /* Current playback position in bytes */
    i32 State;                /* playing or paused */ 
    u32 StreamSize;           /* Size of stream data in bytes */
    u8 Flags;                 /* Bitfield of source properties */
    f32 LGain;                /* Left channel multiplier */
    f32 RGain;                /* Right channel multiplier */

    f32 Level;                /* Set by lcAudioSetLevel */
    f32 Pan;                  /* Set by lcAudioSetPan */
};

internal struct
{
    lcAudioSource_t *Playing; /* Linked list of active sources */
    u32 BufferSize;           /* sizeof(_lcAudio.Buffer) */
    u8 Buffer[0x400];         /* Internal master buffer */
    b8 Running;
} _lcAudio;

internal void _lcAudioProcess(void);

#ifdef LC_PLATFORM_WINDOWS
#error TODO: windows audio
#elif defined LC_PLATFORM_LINUX
#include "Platform/Linux/Audio.c"
#else
#error Platform macro undefined
#endif

internal void
_lcAudioInit(void)
{
    _lcAudio.BufferSize = sizeof(_lcAudio.Buffer);
    _lcAudio.Running = true;

    _lcPlatformAudioInit();
}

internal void
_lcAudioProcessSource(lcAudioSource_t *source)
{
    i32 i;
    i32 mix;


    if (LC_AUDIO_SOURCE_HAS_FLAG(source, LC_SOURCE_FLAG_REWIND))
    {
        source->Playhead = 0;
        source->Flags &= ~LC_SOURCE_FLAG_REWIND;
    }

    if (source->State != LC_AUDIO_STATE_PLAYING)
    {
        return;
    }

    for (i = 0;
         i < (_lcAudio.BufferSize >> 1);
         ++i)
    {
        /* left channel */
        mix = (i32)(((i16*)_lcAudio.Buffer)[i]);
        mix += (i32)((((i16*)(source->Stream + source->Playhead))[i]) *
               source->LGain);
        ((i16*)_lcAudio.Buffer)[i] = (i16)LC_AUDIO_CLAMP(mix, -32768, 32767);

        ++i;

        /* right channel */
        mix = (i32)(((i16*)_lcAudio.Buffer)[i]);
        mix += (i32)((((i16*)(source->Stream + source->Playhead))[i]) *
               source->RGain);
        ((i16*)_lcAudio.Buffer)[i] = (i16)LC_AUDIO_CLAMP(mix, -32768, 32767);
    }

    source->Playhead += _lcAudio.BufferSize;

    if (source->Playhead >= source->StreamSize)
    {
        source->Playhead = 0;

        if (!LC_AUDIO_SOURCE_HAS_FLAG(source, LC_SOURCE_FLAG_LOOPING))
        {
            source->State = LC_AUDIO_STATE_PAUSED;
        }
    }
}

internal void
_lcAudioProcess(void)
{
    lcAudioSource_t **indirect;

    memset(&_lcAudio.Buffer, 0, _lcAudio.BufferSize);

    _lcPlatformAudioLock();
    indirect = &_lcAudio.Playing;
    while (*indirect)
    {
        _lcAudioProcessSource(*indirect);

        if ((*indirect)->State != LC_AUDIO_STATE_PLAYING)
        /* Remove source from list if it is not playing */
        {
            (*indirect)->Flags &= ~LC_SOURCE_FLAG_ACTIVE;
            *indirect = (*indirect)->Next;
        }
        else
        {
            indirect = &((*indirect)->Next);
        }
    }
    _lcPlatformAudioUnlock();
}

void
lcAudioPlay(lcAudioSource_t *source)
{
    _lcPlatformAudioLock();
    source->State = LC_AUDIO_STATE_PLAYING;
    if (!LC_AUDIO_SOURCE_HAS_FLAG(source, LC_SOURCE_FLAG_ACTIVE))
    {
        source->Flags |= LC_SOURCE_FLAG_ACTIVE;
        source->Next = _lcAudio.Playing;
        _lcAudio.Playing = source;
    }
    _lcPlatformAudioUnlock();
}

void
lcAudioPause(lcAudioSource_t *source)
{
    _lcPlatformAudioLock();
    source->State = LC_AUDIO_STATE_PAUSED;
    _lcPlatformAudioUnlock();
}

void
lcAudioStop(lcAudioSource_t *source)
{
    _lcPlatformAudioLock();
    source->Flags |= LC_SOURCE_FLAG_REWIND;
    source->State = LC_AUDIO_STATE_PAUSED;
    _lcPlatformAudioUnlock();
}

void
lcAudioSetLooping(lcAudioSource_t *source,
                  b8 looping)
{
    _lcPlatformAudioLock();
    if (looping)
    {
        source->Flags |= LC_SOURCE_FLAG_LOOPING;
    }
    else
    {
        source->Flags &= ~LC_SOURCE_FLAG_LOOPING;
    }
    _lcPlatformAudioUnlock();
}

internal void
_lcAudioRecalculateGain(lcAudioSource_t *source)
{
    _lcPlatformAudioLock();
    source->LGain = (1.0f - source->Pan) * source->Level;
    source->RGain = source->Pan * source->Level;
    _lcPlatformAudioUnlock();
}

void
lcAudioSetLevel(lcAudioSource_t *source,
                f32 level)
{
    source->Level = level;
    _lcAudioRecalculateGain(source);
}

void
lcAudioSetPan(lcAudioSource_t *source,
              f32 pan)
{
    source->Pan = pan;
    _lcAudioRecalculateGain(source);
}

internal void
_lcAudioDestroy(void)
{
    _lcAudio.Running = false;

    _lcPlatformAudioDestroy();
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

