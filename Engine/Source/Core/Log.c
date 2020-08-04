/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 30 July 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_STRINGIFY(x) #x
#define LC_TO_STRING(x) LC_STRINGIFY(x)
#define __LINE_STRING__ LC_TO_STRING(__LINE__)

#ifdef LC_LOGGING_ENABLED

    #define LC_CORE_LOG_ERROR(...) lcLog(LC_LOG_LEVEL_ERROR, "CORE", __VA_ARGS__)
    #define LC_CORE_LOG_WARN(...)  lcLog(LC_LOG_LEVEL_WARN, "CORE", __VA_ARGS__)
    #define LC_CORE_LOG_INFO(...)  lcLog(LC_LOG_LEVEL_INFO, "CORE", __VA_ARGS__)
    #define LC_CORE_LOG_DEBUG(...) lcLog(LC_LOG_LEVEL_DEBUG, "CORE", __VA_ARGS__)
    #define LC_CORE_LOG_TRACE(...) lcLog(LC_LOG_LEVEL_TRACE, "CORE", __VA_ARGS__)
    
#else

    #define LC_CORE_LOG_ERROR(...)
    #define LC_CORE_LOG_WARN(...)
    #define LC_CORE_LOG_INFO(...)
    #define LC_CORE_LOG_DEBUG(...)
    #define LC_CORE_LOG_TRACE(...)
    
#endif

#if defined(LC_PLATFORM_WINDOWS) && defined(LC_LOGGING_ENABLED)
    #include <windows.h>

    static HANDLE stdoutHandle;
    static DWORD outModeInit;
    
    void
    lcLogInit(void)
    {
        DWORD outMode = 0;
        stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        if (stdoutHandle == INVALID_HANDLE_VALUE)
            exit(GetLastError());

        if(!GetConsoleMode(stdoutHandle, &outMode))
            exit(GetLastError());

        outModeInit = outMode;
        outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        if (!SetConsoleMode(stdoutHandle, outMode))
            exit(GetLastError());
    }

    void
    lcLogDestroy(void)
    {
        printf("\x1b[0m");

        if (!SetConsoleMode(stdoutHandle, outModeInit))
            exit(GetLastError());
    }
#else
    void
    lcLogInit(void)
    {
    }
    
    void
    lcLogDestroy(void)
    {
    }
#endif

static char *
lcLogLevelNames[] =
{
  "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

static char *
lcLogLevelColours[] =
{
  "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};

void
lcLog(int level,
      char *prefix,
      char *fmt, ...)
{    
    va_list args;
    fprintf(
        stderr, "\x1b[0m%s%-5s\x1b[0m \x1b[38m%-6s\x1b[34m │ \x1b[0m ",
        lcLogLevelColours[level], lcLogLevelNames[level], prefix);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    fflush(stderr);
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

