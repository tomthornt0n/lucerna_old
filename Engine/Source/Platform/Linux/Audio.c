/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna

  Author  : Tom Thornton
  Updated : 30 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

internal struct
{
    pthread_mutex_t Lock;
    pthread_t Thread;
    snd_pcm_t *Handle;
    u8 Buffer[1024];
    u32 BufferSize;
} _lcPlatformAudioData;

internal void *
_lcPlatformAudioUpdate(void *arg)
{
    snd_pcm_sframes_t frames;

    while (_lcAudio.Running)
    {
        _lcAudioProcess(_lcPlatformAudioData.Buffer,
                        _lcPlatformAudioData.BufferSize);

        snd_pcm_wait(_lcPlatformAudioData.Handle,
                     LC_AUDIO_SAMPLE_RATE *
                     (_lcPlatformAudioData.BufferSize / 4));

        frames = snd_pcm_writei(_lcPlatformAudioData.Handle,
                                LC_AUDIO_BYTES_TO_SAMPLES(_lcAudio.Buffer));

        if (frames == -EPIPE)
        {
            LC_CORE_LOG_ERROR("Audio buffer underrun occured!");
            snd_pcm_prepare(_lcPlatformAudioData.Handle);
        }
        else if (frames < 0)
        {
            frames = snd_pcm_recover(_lcPlatformAudioData.Handle, frames, 0);
        }

        if (frames < 0)
        {
            LC_CORE_LOG_ERROR("snd_pcm_writei failed: %s",
                              snd_strerror(frames));
        }
    }

    return NULL;
}

internal void
_lcPlatformAudioLock(void)
{
    pthread_mutex_lock(&_lcPlatformAudioData.Lock);
}

internal void
_lcPlatformAudioUnlock(void)
{
    pthread_mutex_unlock(&_lcPlatformAudioData.Lock);
}

internal void
_lcPlatformAudioInit(void)
{
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    memcpy(&_lcPlatformAudioData.Lock, &lock, sizeof(lock));

    _lcPlatformAudioData.BufferSize = sizeof(_lcPlatformAudioData.Buffer);

    LC_ASSERT(snd_pcm_open(&_lcPlatformAudioData.Handle,
                           "default",
                           SND_PCM_STREAM_PLAYBACK,
                           0)
              >= 0,
              "Could not open pcm device");

    LC_ASSERT(snd_pcm_set_params(_lcPlatformAudioData.Handle,
                                 SND_PCM_FORMAT_S16_LE,
                                 SND_PCM_ACCESS_RW_INTERLEAVED,
                                 2,
                                 LC_AUDIO_SAMPLE_RATE,
                                 1,
                                 LC_AUDIO_LATENCEY_IN_US)
              >= 0,
              "Could not configure pcm device");

    pthread_create(&_lcPlatformAudioData.Thread,
                   NULL,
                   _lcPlatformAudioUpdate,
                   NULL);
}


internal void
_lcPlatformAudioDestroy(void)
{
    snd_pcm_drop(_lcPlatformAudioData.Handle);
    snd_pcm_drain(_lcPlatformAudioData.Handle);
    snd_pcm_close(_lcPlatformAudioData.Handle);
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

