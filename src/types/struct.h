#include <v8.h>
#include <node_object_wrap.h>

#include <vector>
#include <glib.h>
#include <girepository.h>

#ifndef GIR_STRUCT_INTERFACE_H
#define GIR_STRUCT_INTERFACE_H

namespace gir {

class GIRStruct;

struct StructFunctionTemplate {
    char *type_name;
    GIObjectInfo *info;
    v8::Persistent<v8::FunctionTemplate> function;
    GType type;
    char *namespace_;
};

struct StructMarshalData {
    GIRStruct *that;
    char *event_name;
};

struct StructData {
    v8::Persistent<v8::Value> instance;
    GIRStruct *structure;
};

class GIRStruct : public node::ObjectWrap {
  public:
    GIRStruct() {};
    GIRStruct(GIObjectInfo *info);
    
    gpointer structure;
    bool abstract;
    GIBaseInfo *info;
    
    static std::vector<StructData> instances;
    static std::vector<StructFunctionTemplate> templates;
    
    static v8::Handle<v8::Value> New(GIStructInfo *info); 
    static v8::Handle<v8::Value> New(const v8::Arguments &args);
    
    static void Prepare(v8::Handle<v8::Object> target, GIObjectInfo *info);
    static void RegisterMethods(v8::Handle<v8::Object> target, GIObjectInfo *info, const char *namespace_, v8::Handle<v8::FunctionTemplate> t); 

    static void Initialize(v8::Handle<v8::Object> target, char *namespace_);
   
    static v8::Handle<v8::Value> CallMethod(const v8::Arguments &args);
    static v8::Handle<v8::Value> CallStaticMethod(const v8::Arguments &args);
    
    static void PushInstance(GIRStruct *obj, v8::Handle<v8::Value>);
    static v8::Handle<v8::Value> GetInstance(GObject *obj);


  private:  
    static v8::Handle<v8::Object> PropertyList(GIObjectInfo *info);
    static v8::Handle<v8::Object> MethodList(GIObjectInfo *info);
    static v8::Handle<v8::Object> InterfaceList(GIObjectInfo *info);
    static v8::Handle<v8::Object> FieldList(GIObjectInfo *info);
    
    static bool ToParams(v8::Handle<v8::Value> val, GParameter** p, int *length, GIObjectInfo *info);
    static void DeleteParams(GParameter* params, int length);
};

}

#endif
