#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

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

namespace gir {
namespace Util {

gchar *utf8StringFromValue(v8::Handle<v8::Value> value)
{
    v8::Local<v8::String> string = value->ToString();
    gchar *buffer = g_new(gchar, string->Utf8Length());
    string->WriteUtf8(buffer);
    return buffer;
}

}
}
