#pragma once

#include <v8.h>
#include <glib.h>
#include <girepository.h>

namespace gir {

class GIRValue {
  public:
    static v8::Handle<v8::Value> FromGValue(GValue *v, GIBaseInfo *base_info);
    static bool ToGValue(v8::Handle<v8::Value> value, GType t, GValue *v);
    static GType GuessType(v8::Handle<v8::Value> value);
};

}
