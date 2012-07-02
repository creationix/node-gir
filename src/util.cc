#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <glib.h>

static char *_format_message(const char *fmt, va_list args)
{
	if(!fmt)
		return g_strdup("");
	
	return g_strdup_vprintf(fmt, args);
}

extern "C" void debug_printf(const char *fmt, ...) 
{
	const char *debug = getenv("NODE_GIR_DEBUG");
	if (debug == NULL || *debug == '\0') {
		return;
	}
	printf(" DEBUG: ");
	
	va_list args;
	va_start (args, fmt);
	char *msg = _format_message(fmt, args);
	va_end(args);

	printf("%s", msg);
	g_free(msg);
}
