/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna

  Author  : Tom Thornton
  Updated : 26 Oct 2020
  License : MIT, at end of file
  Notes   : Potentially the worst code ever written - just enough to hear
            something.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
#include <pthread.h>

static struct
{
  pthread_mutex_t Lock;
  snd_pcm_t *Handle;
  int BufferSize;
} lcAudio;

static
void lcAudioInit(void)
{
  int rc;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;

  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  memcpy(&lcAudio.Lock, &lock, sizeof(lock));

  pthread_mutex_lock(&lcAudio.Lock);

  rc = snd_pcm_open(&lcAudio.Handle, "default",
                    SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0) {
    fprintf(stderr,
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  snd_pcm_hw_params_alloca(&params);

  snd_pcm_hw_params_any(lcAudio.Handle, params);

  snd_pcm_hw_params_set_access(lcAudio.Handle, params,
                      SND_PCM_ACCESS_RW_INTERLEAVED);

  snd_pcm_hw_params_set_format(lcAudio.Handle, params,
                               SND_PCM_FORMAT_S16_LE);

  snd_pcm_hw_params_set_channels(lcAudio.Handle, params, 2);

  val = 44100;
  snd_pcm_hw_params_set_rate_near(lcAudio.Handle,
                                  params, &val, &dir);

  frames = 32;
  snd_pcm_hw_params_set_period_size_near(lcAudio.Handle,
                                         params, &frames, &dir);

  rc = snd_pcm_hw_params(lcAudio.Handle, params);
  if (rc < 0) {
    fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  snd_pcm_hw_params_get_period_size(params, &frames,
                                    &dir);
  lcAudio.BufferSize = frames * 4;

  pthread_mutex_unlock(&lcAudio.Lock);
}

void
lcAudioDestroy(void)
{
      pthread_mutex_lock(&lcAudio.Lock);
      snd_pcm_drain(lcAudio.Handle);
      snd_pcm_close(lcAudio.Handle);
      pthread_mutex_unlock(&lcAudio.Lock);
}

void *
_lcAudioPlay(void *arg)
{
    lcAudioSource_t *source;
    int frames;

    source = (lcAudioSource_t *)arg;

    pthread_mutex_lock(&lcAudio.Lock);
    source->State = LC_AUDIO_PLAYING;

    frames = lcAudio.BufferSize / 4;

    while (source->Playhead < source->BufferSize)
    {
        snd_pcm_wait(lcAudio.Handle, 1000);
        if (snd_pcm_writei(lcAudio.Handle,
                           source->Buffer + 
                           source->Playhead,
                           frames) < 0)
        {
            snd_pcm_prepare(lcAudio.Handle);
            LC_CORE_LOG_ERROR("Buffer underrun");
        }

        source->Playhead += lcAudio.BufferSize;
    }

    snd_pcm_drop(lcAudio.Handle);

    pthread_mutex_unlock(&lcAudio.Lock);
    source->State = LC_AUDIO_PAUSED;
    source->Playhead = 0;

    return NULL;
}

void
lcAudioPlay(lcAudioSource_t *source)
{
    pthread_t newThread;
    pthread_create(&newThread, NULL, _lcAudioPlay, source);
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

