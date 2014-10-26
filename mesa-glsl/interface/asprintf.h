#ifndef ASPRINTF_H
#define ASPRINTF_H

#include <stdarg.h>
#include <stdio.h>

#if !defined(vasprintf)
static int vasprintf(char **s, const char *format, va_list ap)
{
	/* Guess we need no more than 100 bytes. */
	int n, size = 100;
	va_list save_ap;

	if ((*s = (char*) malloc(size)) == NULL)
		return -1;
	while (1) {
		/* wwork on a copy of the va_list because of a bug
		 in the vsnprintf implementation in x86_64 libc
		 */
#ifdef __va_copy
		__va_copy(save_ap, ap);
#else
		save_ap = ap;
#endif
		/* Try to print in the allocated space. */
		n = _vsnprintf(*s, size, format, save_ap);
		va_end(save_ap);
		/* If that worked, return the string. */
		if (n > -1 && n < size) {
			return n;
		}
		/* Else try again with more space. */
		if (n > -1) { /* glibc 2.1 */
			size = n + 1; /* precisely what is needed */
		} else { /* glibc 2.0 */
			size *= 2; /* twice the old size */
		}
		if ((*s = (char*) realloc(*s, size)) == NULL) {
			return -1;
		}
	}
}
#endif

#if !defined(asprintf)
static int asprintf(char **s, const char *format, ...)
{
	va_list vals;
	int result;

	va_start(vals, format);
	result = vasprintf(s, format, vals);
	va_end(vals);
	return result;
}
#endif
#endif

