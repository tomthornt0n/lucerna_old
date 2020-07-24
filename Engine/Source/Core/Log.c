#define _LC_STRINGIFY(x) #x
#define _LC_TO_STRING(x) _LC_STRINGIFY(x)
#define __LINE_STRING__ _LC_TO_STRING(__LINE__)

#if defined(LC_DEBUG) || defined(LC_DO_NOT_STRIP_LOGGING_FROM_RELEASE)
    #define LC_LOGGING_ENABLED
#endif

#ifdef LC_LOGGING_ENABLED

    /* core logging macro wrappers */
    #define LC_CORE_LOG_ERROR(...)      lc_Log(LC_LOG_LEVEL_ERROR, "CORE", __VA_ARGS__)
    #define LC_CORE_LOG_WARN(...)       lc_Log(LC_LOG_LEVEL_WARN, "CORE", __VA_ARGS__)
    #define LC_CORE_LOG_INFO(...)       lc_Log(LC_LOG_LEVEL_INFO, "CORE", __VA_ARGS__)
    #define LC_CORE_LOG_DEBUG(...)      lc_Log(LC_LOG_LEVEL_DEBUG, "CORE", __VA_ARGS__)
    #define LC_CORE_LOG_TRACE(...)      lc_Log(LC_LOG_LEVEL_TRACE, "CORE", __VA_ARGS__)
    
    /* client logging macro wrappers */
    #define LC_LOG_ERROR(...)           lc_Log(LC_LOG_LEVEL_ERROR, "CLIENT", __VA_ARGS__)
    #define LC_LOG_WARN(...)            lc_Log(LC_LOG_LEVEL_WARN, "CLIENT", __VA_ARGS__)
    #define LC_LOG_INFO(...)            lc_Log(LC_LOG_LEVEL_INFO, "CLIENT", __VA_ARGS__)
    #define LC_LOG_DEBUG(...)           lc_Log(LC_LOG_LEVEL_DEBUG, "CLIENT", __VA_ARGS__)
    #define LC_LOG_TRACE(...)           lc_Log(LC_LOG_LEVEL_TRACE, "CLIENT", __VA_ARGS__)

#else

    #define LC_CORE_LOG_ERROR(...)
    #define LC_CORE_LOG_WARN(...)
    #define LC_CORE_LOG_INFO(...)
    #define LC_CORE_LOG_DEBUG(...)
    #define LC_CORE_LOG_TRACE(...)
    
    #define LC_LOG_ERROR(...)
    #define LC_LOG_WARN(...)
    #define LC_LOG_INFO(...)
    #define LC_LOG_DEBUG(...)
    #define LC_LOG_TRACE(...)

#endif

/* asserstions */
#ifdef _MSC_VER
    #define LC_ASSERT(x, ...) { if(!(x)) { lc_Log(LC_LOG_LEVEL_FATAL, "Assertion failure at line "__LINE_STRING__" in "__FILE__, __VA_ARGS__); __debugbreak(); } }
#else
    #define LC_ASSERT(x, ...) { if(!(x)) { lc_Log(LC_LOG_LEVEL_FATAL, "Assertion failure at line "__LINE_STRING__" in "__FILE__, __VA_ARGS__); exit(0);} }
#endif

#if defined(LC_PLATFORM_WINDOWS) && defined(LC_LOGGING_ENABLED)
    #include <windows.h>

    static HANDLE stdoutHandle;
    static DWORD outModeInit;
    
    void
    lc_LogInit(void)
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
    lc_LogDestroy(void)
    {
        printf("\x1b[0m");

        if (!SetConsoleMode(stdoutHandle, outModeInit))
            exit(GetLastError());
    }
#else
    void
    lc_LogInit(void)
    {
    }
    
    void
    lc_LogDestroy(void)
    {
    }
#endif

enum { LC_LOG_LEVEL_TRACE, LC_LOG_LEVEL_DEBUG, LC_LOG_LEVEL_INFO, LC_LOG_LEVEL_WARN, LC_LOG_LEVEL_ERROR, LC_LOG_LEVEL_FATAL };

static const char *lc_LogLevelNames[] =
{
  "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

static const char *lc_LogLevelColours[] =
{
  "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};

void
lc_Log(int level,
       const char *prefix,
       const char *fmt, ...)
{    
    /* Log to stderr */
    va_list args;
    fprintf(
        stderr, "\x1b[0m%s%-5s\x1b[0m \x1b[38m%-6s\x1b[34m │ \x1b[0m ",
        lc_LogLevelColours[level], lc_LogLevelNames[level], prefix);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    fflush(stderr);
}

