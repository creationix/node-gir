#include <v8.h>

#include <glib.h>
#include <girepository.h>

#ifndef GIR_VALUES_H
#define GIR_VALUES_H

namespace gir {

class GIRValue {
  public:
    static v8::Handle<v8::Value> FromGValue(GValue *v, GIBaseInfo *base_info);
    static bool ToGValue(v8::Handle<v8::Value> value, GType t, GValue *v);
    static GType GuessType(v8::Handle<v8::Value> value);
};

}

#endif
