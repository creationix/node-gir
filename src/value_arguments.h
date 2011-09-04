#include <v8.h>

#include <glib.h>
#include <girepository.h>

#ifndef GIR_VALUE_ARGUMENTS_H
#define GIR_VALUE_ARGUMENTS_H

namespace gir {

class ValueToArgs {
  public:
    static v8::Handle<v8::Value> CallFunc(GObject *obj, GIFunctionInfo *info, const v8::Arguments &args);
    static bool Convert(v8::Handle<v8::Value> v, GIArgument *arg, GIArgInfo *info);
    static v8::Handle<v8::Value> ArgToValue(GIArgument *arg, GITypeInfo *type);
};

}

#endif
