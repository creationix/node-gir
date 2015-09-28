#pragma once

#include <v8.h>
#include <nan.h>
#include <glib.h>
#include <girepository.h>
#include <vector>

namespace gir {

class GIRStruct;

struct StructFunctionTemplate {
    char *type_name;
    GIObjectInfo *info;
    v8::Handle<v8::FunctionTemplate> function;
    GType type;
    char *namespace_;
};

struct StructMarshalData {
    GIRStruct *that;
    char *event_name;
};

struct StructData {
    v8::Handle<v8::Value> instance;
    GIRStruct *gir_structure;
};

class GIRStruct : public Nan::ObjectWrap {
  public:
    GIRStruct() {};
    GIRStruct(GIObjectInfo *info);

    gpointer c_structure;
    bool abstract;
    GIBaseInfo *info;

    static std::vector<StructData> instances;
    static std::vector<StructFunctionTemplate> templates;

    static v8::Handle<v8::Value> New(gpointer c_structure, GIStructInfo *info);
    static NAN_METHOD(New);

    static void Prepare(v8::Handle<v8::Object> target, GIObjectInfo *info);
    static void RegisterMethods(v8::Handle<v8::Object> target, GIObjectInfo *info, const char *namespace_, v8::Handle<v8::FunctionTemplate> t);

    static void Initialize(v8::Handle<v8::Object> target, char *namespace_);

    static NAN_METHOD(CallMethod);

    static void PushInstance(GIRStruct *obj, v8::Handle<v8::Value>);
    static v8::Handle<v8::Value> GetStructure(gpointer c_structure);


  private:
    static v8::Handle<v8::Object> PropertyList(GIObjectInfo *info);
    static v8::Handle<v8::Object> MethodList(GIObjectInfo *info);
    static v8::Handle<v8::Object> InterfaceList(GIObjectInfo *info);
    static v8::Handle<v8::Object> FieldList(GIObjectInfo *info);
};

}
