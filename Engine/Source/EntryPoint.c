/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 30 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

i32
main(i32 argc,
     i8 **argv)
{
#ifdef LC_PLATFORM_WINDOWS
    return WinMain(GetModuleHandle(NULL),
                   NULL,
                   GetCommandLine(),
                   SW_SHOWNORMAL);
#elif defined LC_PLATFORM_LINUX
    lcInitConfig_t config;

    config = lcClientConfig();

    _lcGLLoad();
    _lcInputInit();
    _lcMessageSystemInit();
    _lcWindowInit(config.WindowTitle,
                  config.WindowDimensions[0],
                  config.WindowDimensions[1],
                  config.VSyncEnabled);
    _lcRendererInit();
    _lcLoadMasterTexture();
    _lcCameraInit(config.CameraPosition);
    _lcAudioInit();

    lcClientMain(argc, argv);

    _lcAudioDestroy();
    _lcRendererDestroy();
    _lcCameraDestroy();
    _lcWindowDestroy();
    _lcMessageSystemDestroy();

    return 0;
#else
#error No platform macro defined
#endif
}

#ifdef LC_PLATFORM_WINDOWS
i32
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        i32 nShowCmd)
{
    lcInitConfig_t config;

    lcClockInit();

    config = lcClientConfig();

#ifdef LC_LOGGING_ENABLED
    lcLogInitWindowsConsole();
#endif
    _lcInputInit();
    _lcMessageSystemInit();
    _lcWindowInit(hInstance,
                  config.WindowTitle,
                  config.WindowDimensions[0],
                  config.WindowDimensions[1],
                  config.VSyncEnabled);
    _lcGLLoad();
    _lcRendererInit();
    _lcLoadMasterTexture();
    _lcCameraInit(config.CameraPosition);
    _lcAudioInit();

    lcClientMain(__argc, __argv);

    _lcAudioDestroy();
    _lcRendererDestroy();
    _lcCameraDestroy();
    _lcMessageSystemDestroy();

    return 0;
}
#endif

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

