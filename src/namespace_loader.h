#include <v8.h>
#include <map>
#include <glib.h>
#include <girepository.h>

#ifndef GIR_NAMESPACE_LOADER_H
#define GIR_NAMESPACE_LOADER_H

namespace gir {

class NamespaceLoader {
  public:
    static GIRepository *repo;
    static std::map<char *, GITypelib*> type_libs;
    
    static void Initialize(v8::Handle<v8::Object> target);
    static v8::Handle<v8::Value> Load(const v8::Arguments &args);
    
    static v8::Handle<v8::Value> SearchPath(const v8::Arguments &args);

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

#endif
