#include <v8.h>
#include <node_object_wrap.h>

#include <map>
#include <glib.h>
#include <girepository.h>

#ifndef GIR_FUNCTION_INTERFACE_H
#define GIR_FUNCTION_INTERFACE_H

namespace gir {

class GIRFunction : public node::ObjectWrap {
  public:
    GIRFunction() {};
    
    static void Initialize(v8::Handle<v8::Object> target, GIObjectInfo *info);
    static v8::Handle<v8::Value> Execute(const v8::Arguments &args);
    static char* ToCamelCase(const char *str);

  private:
};

}

#endif

