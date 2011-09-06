#include <v8.h>
#include <node_object_wrap.h>

#include <map>
#include <glib.h>
#include <girepository.h>

#ifndef GIR_OBJECT_INTERFACE_H
#define GIR_OBJECT_INTERFACE_H

namespace gir {

class GIRObject : public node::ObjectWrap {
  public:
    GIRObject() {};
    GIRObject(GIObjectInfo *info_);
    
    void constructor(GIObjectInfo *info_);
    
    GObject *obj;
    bool abstract;
    GIBaseInfo *info;
    static std::map<char*, GIObjectInfo*> objects;
    static std::map<GIObjectInfo*, v8::Persistent<v8::FunctionTemplate> > templates;
    
    static v8::Handle<v8::Value> New(GObject *obj, GIObjectInfo *info);
    static v8::Handle<v8::Value> New(GIPropertyInfo *prop_);
    static v8::Handle<v8::Value> New(const v8::Arguments &args);
    static void Prepare(v8::Handle<v8::Object> target, GIObjectInfo *info);
    static void SetPrototypeMethods(v8::Handle<v8::FunctionTemplate> t, char *name);
    
    static void Initialize(v8::Handle<v8::Object> target);
    
    static v8::Handle<v8::Value> CallMethod(const v8::Arguments &args);
    static v8::Handle<v8::Value> GetProperty(const v8::Arguments &args);
    static v8::Handle<v8::Value> SetProperty(const v8::Arguments &args);
    static v8::Handle<v8::Value> GetInterface(const v8::Arguments &args);
    static v8::Handle<v8::Value> GetField(const v8::Arguments &args);
    static v8::Handle<v8::Value> WatchSignal(const v8::Arguments &args);
    static v8::Handle<v8::Value> CallVFunc(const v8::Arguments &args);

  private:
    GIFunctionInfo *FindMethod(GIObjectInfo *inf, char *name);
    GIFunctionInfo *FindProperty(GIObjectInfo *inf, char *name);
    GIFunctionInfo *FindInterface(GIObjectInfo *inf, char *name);
    GIFunctionInfo *FindField(GIObjectInfo *inf, char *name);
    GIFunctionInfo *FindSignal(GIObjectInfo *inf, char *name);
    GIFunctionInfo *FindVFunc(GIObjectInfo *inf, char *name);
    
    static v8::Handle<v8::Object> PropertyList(GIObjectInfo *info);
    static v8::Handle<v8::Object> MethodList(GIObjectInfo *info);
    static v8::Handle<v8::Object> InterfaceList(GIObjectInfo *info);
    static v8::Handle<v8::Object> FieldList(GIObjectInfo *info);
    static v8::Handle<v8::Object> SignalList(GIObjectInfo *info);
    static v8::Handle<v8::Object> VFuncList(GIObjectInfo *info);
};

}

#endif
