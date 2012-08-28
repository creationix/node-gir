#include <v8.h>

#include <glib.h>
#include <girepository.h>

#ifndef GIR_FUNCTION_H
#define GIR_FUNCTION_H

namespace gir {

class Func {
  public:
    static v8::Handle<v8::Value> Call(GObject *obj, GIFunctionInfo *info, const v8::Arguments &args, bool ignore_function_name);
    static v8::Handle<v8::Value> CallAndGetPtr(GObject *obj, GIFunctionInfo *info, const v8::Arguments &args, bool ignore_function_name, GIArgument *retval, GITypeInfo **returned_type_info, gint *returned_array_length);
    static v8::Handle<v8::Value> CallStaticMethod(const v8::Arguments &args);
};

}

#endif
