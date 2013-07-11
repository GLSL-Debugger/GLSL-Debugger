#ifndef BUILD_CONFIG_H
#define BUILD_CONFIG_H

#define MAX_NOTIFY_SIZE 512
#define MAX_BACKTRACE_DEPTH 100

// define for suppressing "defined but not used" warnings
#ifdef __GNUC__
#define UNUSED __attribute__ ((unused))
#else
#define UNUSED
#endif

#endif
