#include "notify.h"
#include "build-config.h"

#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>


static struct {
    severity_t level;
    int log_to_file;
    FILE *file;
    char filename[256];
} utils_notify_settings = {LV_DEBUG, 0, 0, {0}};

int check_notify_file()
{
	if(utils_notify_settings.file == 0) {
		utils_notify_settings.file = fopen(utils_notify_settings.filename, "a+");
		if (!utils_notify_settings.file) {
			utils_notify_settings.log_to_file = 0;
			fprintf(stderr, "Error: unable to open log file %s. File logging disabled.\n", utils_notify_settings.filename);
			return 0;
		}
	}
	return 1;
}
int utils_notify_to_file(const int* value)
{
	if(value)
		utils_notify_settings.log_to_file = *value;
	return utils_notify_settings.log_to_file;
}
const char* utils_notify_filename(const char* filename)
{
	if(filename) {
		strncpy(utils_notify_settings.filename, filename, 256);
	}
	return utils_notify_settings.filename;
}
severity_t utils_notify_level(const severity_t* value)
{
	if(value)
		utils_notify_settings.level = *value;
	return utils_notify_settings.level;
}
void utils_notify_startup()
{
	if(utils_notify_settings.log_to_file)
		check_notify_file();
}
void utils_notify_shutdown()
{
	if(utils_notify_settings.file) {
		fclose(utils_notify_settings.file);
		utils_notify_settings.file = 0;
	}
}
void utils_notify_va(const severity_t sev, const char* filename, unsigned int line, const char *fmt, ...)
{
	if(sev > utils_notify_settings.level)
		return;

	va_list list;
	static char p[12];
	static char prefix[128];
	static char msg[MAX_NOTIFY_SIZE];
	static char t[22];
    time_t timeval = time(0);
    struct tm *tp = localtime(&timeval);
    snprintf(t, 22, "%4d-%02d-%02d %02d:%02d:%02d", 1900+tp->tm_year, tp->tm_mon+1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
	switch (sev) {
		case LV_TRACE:
			strcpy(p, " [TRACE]::");
			break;
		case LV_DEBUG:
			strcpy(p, " [DEBUG]::");
			break;
		case LV_INFO:
			strcpy(p, " [INFO ]::");
			break;
		case LV_WARN:
			strcpy(p, " [WARN ]::");
			break;
		case LV_ERROR:
			strcpy(p, " [ERROR]::");
			break;
		case LV_FATAL:
			strcpy(p, " [FATAL]::");
			break;
		default:
			strcpy(p, " [DEF  ]::");
			break;
	}

	snprintf(prefix, 128, "%s%s%s::%d: ", t, p, filename, line);
	
	va_start(list, fmt);
    vsnprintf(msg, MAX_NOTIFY_SIZE, fmt, list);
    va_end(list);

#if defined GLSLDB_WINDOWS
    OutputDebugStringA(prefix);
    OutputDebugStringA(msg);
#else
    if(utils_notify_settings.log_to_file && check_notify_file())
    	fprintf(utils_notify_settings.file, "%s%s\n", prefix, msg);
    else
    	fprintf(stdout, "%s%s\n", prefix, msg);
#endif
}