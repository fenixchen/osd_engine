#pragma once

#ifdef WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define TV_LOG_VERBOSE 0
#define TV_LOG_INFO    1
#define TV_LOG_WARN    2
#define TV_LOG_ERROR   3
#define TV_LOG_FATAL   4
#define TV_LOG_NONE    0xFF

#ifdef TV_LOG_LEVEL
#define _TV_LOG_LEVEL TV_LOG_LEVEL
#else
#define _TV_LOG_LEVEL TV_LOG_VERBOSE
#endif


extern int _tv_log_global_output_lvl;

#if defined(TV_LOG_OUTPUT_LEVEL)
#define _TV_LOG_OUTPUT_LEVEL TV_LOG_OUTPUT_LEVEL
#else
#define _TV_LOG_OUTPUT_LEVEL _tv_log_global_output_lvl
#endif


#define TV_LOG_ENABLED(lvl)     ((lvl) >= _TV_LOG_LEVEL)
#define TV_LOG_ENABLED_VERBOSE  TV_LOG_ENABLED(TV_LOG_VERBOSE)
#define TV_LOG_ENABLED_DEBUG    TV_LOG_ENABLED(TV_LOG_DEBUG)
#define TV_LOG_ENABLED_INFO     TV_LOG_ENABLED(TV_LOG_INFO)
#define TV_LOG_ENABLED_WARN     TV_LOG_ENABLED(TV_LOG_WARN)
#define TV_LOG_ENABLED_ERROR    TV_LOG_ENABLED(TV_LOG_ERROR)
#define TV_LOG_ENABLED_FATAL    TV_LOG_ENABLED(TV_LOG_FATAL)

#define TV_LOG_ON(lvl) (TV_LOG_ENABLED((lvl)) && (lvl) >= _TV_LOG_OUTPUT_LEVEL)

extern void tv_log_write(int level, const char *func,
                         const char *filename, int line,
                         const char *fmt, ...);

#define TV_LOG_WRITE(level, ...)									\
	do {															\
		if (TV_LOG_ON(level)){										\
			tv_log_write(level, __FUNCTION__, __FILENAME__, __LINE__, __VA_ARGS__);	\
		}															\
	} while (0)


#define TV_LOGV(...) TV_LOG_WRITE(TV_LOG_VERBOSE, __VA_ARGS__)
#define TV_LOGI(...) TV_LOG_WRITE(TV_LOG_INFO, __VA_ARGS__)
#define TV_LOGW(...) TV_LOG_WRITE(TV_LOG_WARN, __VA_ARGS__)
#define TV_LOGE(...) TV_LOG_WRITE(TV_LOG_ERROR, __VA_ARGS__)
#define TV_LOGF(...) TV_LOG_WRITE(TV_LOG_FATAL, __VA_ARGS__)
