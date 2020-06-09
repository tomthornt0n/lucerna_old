//core logging macro wrappers
#define LC_CORE_ERROR(...)                                   lc_Log(LC_LOG_LEVEL_ERROR, "CORE", __VA_ARGS__)
#define LC_CORE_WARN(...)                                    lc_Log(LC_LOG_LEVEL_WARN, "CORE", __VA_ARGS__)
#define LC_CORE_INFO(...)                                    lc_Log(LC_LOG_LEVEL_INFO, "CORE", __VA_ARGS__)
#define LC_CORE_LOG(...)                                     lc_Log(LC_LOG_LEVEL_DEBUG, "CORE", __VA_ARGS__)
#define LC_CORE_TRACE(...)                                   lc_Log(LC_LOG_LEVEL_TRACE, "CORE", __VA_ARGS__)

//client logging macro wrappers
#define LC_ERROR(...)                                        lc_Log(LC_LOG_LEVEL_ERROR, "CLIENT", __VA_ARGS__)
#define LC_WARN(...)                                         lc_Log(LC_LOG_LEVEL_WARN, "CLIENT", __VA_ARGS__)
#define LC_INFO(...)                                         lc_Log(LC_LOG_LEVEL_INFO, "CLIENT", __VA_ARGS__)
#define LC_LOG(...)                                          lc_Log(LC_LOG_LEVEL_DEBUG, "CLIENT", __VA_ARGS__)
#define LC_TRACE(...)                                        lc_Log(LC_LOG_LEVEL_TRACE, "CLIENT", __VA_ARGS__)

#if defined(LC_RELEASE) && !defined(LC_DO_NOT_STRIP_LOGGING_FROM_RELEASE)

#define LC_CORE_ERROR(...)
#define LC_CORE_WARN(...)
#define LC_CORE_INFO(...)
#define LC_CORE_TRACE(...)

#define LC_ERROR(...)
#define LC_WARN(...)
#define LC_INFO(...)
#define LC_TRACE(...)

#endif

#if defined(LC_DEBUG) || defined(LC_DO_NOT_STRIP_LOGGING_FROM_RELEASE)
    #define LC_ENABLE_ASSERTS
#endif

#ifdef LC_ENABLE_ASSERTS
    #ifdef LC_PLATFORM_WINDOWS
        #define LC_ASSERT(x, ...) { if(!(x)) { lc_Log(LC_LOG_LEVEL_FATAL, "ASSERTION FAILURE", __VA_ARGS__); __debugbreak(); } }
    #else
        #define LC_ASSERT(x, ...) { if(!(x)) { lc_Log(LC_LOG_LEVEL_FATAL, "ASSERTION FAILURE", __VA_ARGS__); exit(0);} }
    #endif
#else
    #define LC_ASSERT(x, ...)
    #define LC_ASSERT(x, ...)
#endif

#ifdef LC_PLATFORM_WINDOWS
    #include <windows.h>

    static HANDLE stdoutHandle;
    static DWORD outModeInit;
    
    void lc_InitLog()
    {
        DWORD outMode                                       = 0;
        stdoutHandle                                        = GetStdHandle(STD_OUTPUT_HANDLE);

        if (stdoutHandle == INVALID_HANDLE_VALUE)
            exit(GetLastError());

        if(!GetConsoleMode(stdoutHandle, &outMode))
            exit(GetLastError());

        outModeInit                                         = outMode;
        outMode                                            |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        if (!SetConsoleMode(stdoutHandle, outMode))
            exit(GetLastError());
    }

    void lc_DestroyLog()
    {
        printf("\x1b[0m");

        if (!SetConsoleMode(stdoutHandle, outModeInit))
            exit(GetLastError());
    }
#else
    void lc_InitLog()
    {
    }
    
    void lc_DestroyLog()
    {
        printf("\x1b[0m");
    }
#endif

enum { LC_LOG_LEVEL_TRACE, LC_LOG_LEVEL_DEBUG, LC_LOG_LEVEL_INFO, LC_LOG_LEVEL_WARN, LC_LOG_LEVEL_ERROR, LC_LOG_LEVEL_FATAL };


static const char* lc_LogLevelNames[] =
{
  "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

static const char* lc_LogLevelColours[] =
{
  "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};

void lc_Log(int level, const char *prefix, const char *fmt, ...)
{    
    //Get current time
    time_t t                                               = time(NULL);
    struct tm *lt                                          = localtime(&t);

    //Log to stderr
    va_list args;
    char buf[16];
    buf[strftime(buf, sizeof(buf), "%H:%M:%S", lt)]        = '\0';

    fprintf(
        stderr, "%s %s%-5s\x1b[0m \x1b[90m%s:\x1b[0m ",
        buf, lc_LogLevelColours[level], lc_LogLevelNames[level], prefix);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    fflush(stderr);
}
