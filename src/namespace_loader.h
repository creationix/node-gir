#pragma once

#include <map>
#include <v8.h>
#include <glib.h>
#include <girepository.h>
#include <nan.h>

namespace gir {

class NamespaceLoader : public Nan::ObjectWrap {
  public:
    static GIRepository *repo;
    static std::map<char *, GITypelib*> type_libs;

    static NAN_METHOD(Load);
    static NAN_METHOD(SearchPath);

  private:
    static v8::Handle<v8::Value> LoadNamespace(char *namespace_, char *version);
    static v8::Handle<v8::Value> BuildClasses(char *namespace_);

    static void ParseStruct(GIStructInfo *info, v8::Handle<v8::Object> &exports);
    static void ParseEnum(GIEnumInfo *info, v8::Handle<v8::Object> &exports);
    static void ParseFlags(GIEnumInfo *info, v8::Handle<v8::Object> &exports);
    static void ParseInterface(GIInterfaceInfo *info, v8::Handle<v8::Object> &exports);
    static void ParseUnion(GIUnionInfo *info, v8::Handle<v8::Object> &exports);
    static void ParseFunction(GIFunctionInfo *info, v8::Handle<v8::Object> &exports);

};

}
