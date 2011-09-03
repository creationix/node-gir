#include <v8.h>
#include <node_object_wrap.h>

#include <map>
#include <glib.h>
#include <girepository.h>

#ifndef GIR_OBJECT_H
#define GIR_OBJECT_H

namespace gir {

class GIRObject : public node::ObjectWrap {
  public:
    GIRObject(GIObjectInfo *info);
    
    GObject *obj;
    bool abstract;
    GIBaseInfo *info;
    static std::map<char*, GIObjectInfo*> objects;
    
    static v8::Handle<v8::Value> New(const v8::Arguments &args);
    static void Initialize(v8::Handle<v8::Object> target, GIObjectInfo *info);
    static void SetPrototypeMethods(v8::Handle<v8::FunctionTemplate> t, char *name);
    
    static v8::Handle<v8::Value> Unref(const v8::Arguments &args);
    static v8::Handle<v8::Value> Ref(const v8::Arguments &args);
    static v8::Handle<v8::Value> CallMethod(const v8::Arguments &args);
    static v8::Handle<v8::Value> GetProperty(const v8::Arguments &args);
    static v8::Handle<v8::Value> SetProperty(const v8::Arguments &args);
    static v8::Handle<v8::Value> GetInterface(const v8::Arguments &args);
    static v8::Handle<v8::Value> GetField(const v8::Arguments &args);

  private:
    GIFunctionInfo *FindMethod(GIObjectInfo *inf, char *name);
    GIFunctionInfo *FindProperty(GIObjectInfo *inf, char *name);
    GIFunctionInfo *FindInterface(GIObjectInfo *inf, char *name);
    GIFunctionInfo *FindField(GIObjectInfo *inf, char *name);
    
    static v8::Handle<v8::Object> PropertyList(GIObjectInfo *info);
    static v8::Handle<v8::Object> MethodList(GIObjectInfo *info);
    static v8::Handle<v8::Object> InterfaceList(GIObjectInfo *info);
    static v8::Handle<v8::Object> FieldList(GIObjectInfo *info);
};

}

#endif
