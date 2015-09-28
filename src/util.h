#pragma once

#include <v8.h>
#include <glib.h>

#define GIR_DEFINE_CONSTANT(target, name, constant)                      \
  (target)->Set(v8::String::NewSymbol(name),                              \
            v8::Integer::New(constant),                               \
            static_cast<v8::PropertyAttribute>(v8::ReadOnly|v8::DontDelete))

#define BAD_ARGS(_msg) \
    Nan::ThrowTypeError(_msg);

#define NO_UNDERLYING_OBJECT() \
    Nan::ThrowTypeError("no underlying object found");

#define EXCEPTION(str) \
    Nan::ThrowTypeError(str);

extern "C" void debug_printf(const char *fmt, ...);

namespace gir {

namespace Util {
    gchar *utf8StringFromValue(v8::Handle<v8::Value> value);
}

}
