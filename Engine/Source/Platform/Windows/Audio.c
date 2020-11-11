/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna

  Author  : Tom Thornton
  Updated : 30 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

internal struct
{
    LPDIRECTSOUND DirectSound;
    u32 SecondaryBufferSize;
    LPDIRECTSOUNDBUFFER SecondaryBuffer;
    CRITICAL_SECTION Lock;
} _lcPlatformAudioData;

internal DWORD WINAPI
_lcPlatformAudioUpdate(LPVOID lpParameter)
{
    /* alias to save typing */
    LPDIRECTSOUNDBUFFER sb = _lcPlatformAudioData.SecondaryBuffer;

    u32 outputByteIndex = 0; 

    sb->lpVtbl->Play(sb, 0, 0, DSBPLAY_LOOPING);

    while (_lcAudio.Running)
    {
        DWORD playCursor, writeCursor, targetCursor;

        if (SUCCEEDED(sb->lpVtbl->GetCurrentPosition(sb,
                                                     &playCursor,
                                                     &writeCursor)))
        {
            DWORD byteToLock, bytesToWrite;
            DWORD regionOneSize, regionTwoSize;
            VOID *regionOne, *regionTwo;

            byteToLock = outputByteIndex %
                         _lcPlatformAudioData.SecondaryBufferSize;

            targetCursor = (playCursor +
                            LC_AUDIO_SAMPLES_TO_BYTES(LC_AUDIO_LATENCEY_SAMPLES)) %
                           _lcPlatformAudioData.SecondaryBufferSize;

            if (byteToLock > targetCursor)
            {
                bytesToWrite = (_lcPlatformAudioData.SecondaryBufferSize -
                               byteToLock) +
                               targetCursor;
            }
            else
            {
                bytesToWrite = targetCursor - byteToLock;
            }

            if (SUCCEEDED(sb->lpVtbl->Lock(sb,
                                           byteToLock,
                                           bytesToWrite,
                                           &regionOne,
                                           &regionOneSize,
                                           &regionTwo,
                                           &regionTwoSize,
                                           0)))
            {
                _lcAudioProcess(regionOne, regionOneSize);
                _lcAudioProcess(regionTwo, regionTwoSize);

                sb->lpVtbl->Unlock(sb,
                                   regionOne,
                                   regionOneSize,
                                   regionTwo,
                                   regionTwoSize);

                outputByteIndex += bytesToWrite;
            }
        }
    }
                
    return 0;
}

internal void
_lcPlatformAudioLock(void)
{
    EnterCriticalSection(&_lcPlatformAudioData.Lock);
}

internal void
_lcPlatformAudioUnlock(void)
{
    LeaveCriticalSection(&_lcPlatformAudioData.Lock);
}

internal void
_lcPlatformAudioInit(void)
{
    b8 initialised = true;

    _lcPlatformAudioData.SecondaryBufferSize = 88200;

    InitializeCriticalSection(&_lcPlatformAudioData.Lock);

    if (SUCCEEDED(DirectSoundCreate(0,
                                    &_lcPlatformAudioData.DirectSound,
                                    0)))
    {
        /* alias to save me some typing */
        LPDIRECTSOUND ds = _lcPlatformAudioData.DirectSound;

        if (SUCCEEDED(ds->lpVtbl->SetCooperativeLevel(ds,
                                                      _lcWindow.WindowHandle,
                                                      DSSCL_PRIORITY)))
        {
            WAVEFORMATEX waveFormat;
            memset(&waveFormat, 0, sizeof(waveFormat));
            waveFormat.wFormatTag = WAVE_FORMAT_PCM;
            waveFormat.nChannels = 2;
            waveFormat.nSamplesPerSec = LC_AUDIO_SAMPLE_RATE;
            waveFormat.wBitsPerSample = 16;
            waveFormat.nBlockAlign = (waveFormat.wBitsPerSample *
                                      waveFormat.nChannels) / 8;
            waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign *
                                         waveFormat.nSamplesPerSec;

            /* try to create the primary buffer */
            DSBUFFERDESC primaryBufferDesc;
            memset(&primaryBufferDesc, 0, sizeof(primaryBufferDesc));
            primaryBufferDesc.dwSize = sizeof(primaryBufferDesc);
            primaryBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

            LPDIRECTSOUNDBUFFER primaryBuffer;
            if (SUCCEEDED(ds->lpVtbl->CreateSoundBuffer(ds,
                                                        &primaryBufferDesc,
                                                        &primaryBuffer,
                                                        0)))
            {
                if (!SUCCEEDED(primaryBuffer->lpVtbl->SetFormat(primaryBuffer,
                                                                &waveFormat)))
                {
                    LC_CORE_LOG_ERROR("AUDIO: Could not set buffer format!");
                    initialised = false;
                }
            }
            else
            {
                LC_CORE_LOG_ERROR("AUDIO: Could not create primary buffer!");
                initialised = false;
            }

            /* NOTE(tbt): try to create a secondary buffer (the one we actualy
               write to)
            */
            DSBUFFERDESC secondaryBufferDesc;
            memset(&secondaryBufferDesc, 0, sizeof(secondaryBufferDesc));
            secondaryBufferDesc.dwSize = sizeof(secondaryBufferDesc);
            secondaryBufferDesc.dwBufferBytes = _lcPlatformAudioData.SecondaryBufferSize;
            secondaryBufferDesc.lpwfxFormat = &waveFormat;

            LPDIRECTSOUNDBUFFER secondaryBuffer;
            if (SUCCEEDED(ds->lpVtbl->CreateSoundBuffer(ds,
                                                        &secondaryBufferDesc,
                                                        &secondaryBuffer,
                                                        0)))
            {
                _lcPlatformAudioData.SecondaryBuffer = secondaryBuffer;
            }
            else
            {
                LC_CORE_LOG_ERROR("AUDIO: Could not create secondary buffer!");
                initialised = false;
            }
        }
        else
        {
            LC_CORE_LOG_ERROR("AUDIO: Could not create primary buffer!");
            initialised = false;
        }
    }
    else
    {
        LC_CORE_LOG_ERROR("AUDIO: Could not create DirectSound object!");
        initialised = false;
    }

    LC_ASSERT(initialised, "Could not initialise DirectSound!");

    CreateThread(NULL, 0, _lcPlatformAudioUpdate, NULL, 0, NULL);
}

internal void
_lcPlatformAudioDestroy(void)
{
    DeleteCriticalSection(&_lcPlatformAudioData.Lock);
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

