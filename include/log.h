#ifndef _SCURVY_LOG_H
#define _SCURVY_LOG_H

#include <stdarg.h>

enum log_level {
	L_ERROR = 1,
	L_INFO = 2,
	L_DEBUG = 3
};

void init_log(enum log_level level);

void _scurvy_log(const char *filename, int line, enum log_level level,
		const char* format, ...) __attribute__((format(printf,4,5)));
#define scurvy_log(VERBOSITY, FMT, ...) \
	_scurvy_log(__FILE__, __LINE__, VERBOSITY, FMT, ##__VA_ARGS__)

void _scurvy_vlog(const char *filename, int line, enum log_level level,
		const char* format, va_list ap);
#define scurvy_vlog(VERBOSITY, FMT, ARGS) \
	_scurvy_vlog(__FILE__, __LINE__, VERBOSITY, FMT, ARGS)

#endif
