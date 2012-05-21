#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

extern "C" void debug_printf(const char *fmt, ...) 
{
	const char *debug = getenv("NODE_GIR_DEBUG");
	if (debug == NULL || *debug == '\0') {
		return;
	}
	printf(" DEBUG: ");
	va_list args;
	va_start (args, fmt);
	printf(fmt, args);
	va_end (args);
}
