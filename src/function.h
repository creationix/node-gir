#pragma once

#include <v8.h>
#include <nan.h>
#include <glib.h>
#include <girepository.h>

namespace gir {

class Func {
  public:
    static v8::Local<v8::Value> Call(GObject *obj, GIFunctionInfo *info, const Nan::FunctionCallbackInfo<v8::Value>&args, bool ignore_function_name);
    static v8::Handle<v8::Value> CallAndGetPtr(GObject *obj, GIFunctionInfo *info, const Nan::FunctionCallbackInfo<v8::Value>&args, bool ignore_function_name, GIArgument *retval, GITypeInfo **returned_type_info, gint *returned_array_length);
    static NAN_METHOD(CallStaticMethod);
};

}
