#ifndef GIR_UTIL_H
#define GIR_UTIL_H

#include <glib.h>
#include <v8.h>

#define GIR_DEFINE_CONSTANT(target, name, constant)                      \
  (target)->Set(v8::String::NewSymbol(name),                              \
            v8::Integer::New(constant),                               \
            static_cast<v8::PropertyAttribute>(v8::ReadOnly|v8::DontDelete))

#define BAD_ARGS(_msg) \
    ThrowException(Exception::TypeError(String::New(_msg)));

#define NO_UNDERLYING_OBJECT() \
    ThrowException(Exception::Error(String::New("no underlying object found")));

#define EXCEPTION(str) \
    ThrowException(Exception::Error(String::New(str)));

extern "C" void debug_printf(const char *fmt, ...);

namespace gir {

namespace Util {
    gchar *utf8StringFromValue(v8::Handle<v8::Value> value);
}

}

#endif
