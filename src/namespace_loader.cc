#include "namespace_loader.h"
#include "util.h"

#include "types/object.h"
#include "types/function.h"

#include <string.h>

using namespace v8;

namespace gir {

GIRepository *NamespaceLoader::repo = NULL;
std::map<char *, GITypelib*> NamespaceLoader::type_libs;

void NamespaceLoader::Initialize(Handle<Object> target) {
    GIR_SET_METHOD(target, "import", NamespaceLoader::Import);
    GIR_SET_METHOD(target, "searchPath", NamespaceLoader::SearchPath);
    GIR_SET_METHOD(target, "isRegistered", NamespaceLoader::IsRegistered);
    GIR_SET_METHOD(target, "getDependencies", NamespaceLoader::GetDependencies);
    GIR_SET_METHOD(target, "loadedNamespaces", NamespaceLoader::LoadedNamespaces);
    GIR_SET_METHOD(target, "getVersion", NamespaceLoader::GetVersion);
    GIR_SET_METHOD(target, "getVersions", NamespaceLoader::GetVersions);
}

Handle<Value> NamespaceLoader::Import(const Arguments &args) {
    HandleScope scope;
    
    if(args.Length() < 1) {
        EXCEPTION("too few arguments");
    }
    if(!args[0]->IsString()) {
        EXCEPTION("argument has to be a string");
    }
    
    Handle<Value> exports;
    String::Utf8Value namespace_(args[0]);
    
    if(args.Length() > 1 && args[1]->IsString()) {
        String::Utf8Value version(args[1]);
        exports = NamespaceLoader::LoadNamespace(*namespace_, *version);
    }
    else {
        exports = NamespaceLoader::LoadNamespace(*namespace_, NULL);
    }
    
    
    return scope.Close(exports);
}

Handle<Value> NamespaceLoader::LoadNamespace(char *namespace_, char *version) {
    if(!repo) {
        repo = g_irepository_get_default();
    }
    
    GError *er = NULL;
    GITypelib *lib = g_irepository_require(repo, namespace_, version, (GIRepositoryLoadFlags)0, &er);
    if(!lib) {
        return EXCEPTION(er->message);
    }
    char *active_namespace = new char[strlen(namespace_)];
    strcpy(active_namespace, namespace_);
    
    type_libs.insert(std::make_pair(namespace_, lib));
    
    Handle<Value> res = BuildClasses(active_namespace);
    delete[] active_namespace;
    return res;
}

Handle<Value> NamespaceLoader::BuildClasses(char *namespace_) {
    Handle<Object> exports = Object::New();
    
    int length = g_irepository_get_n_infos(repo, namespace_);
    for(int i=0; i<length; i++) {
        GIBaseInfo *info = g_irepository_get_info(repo, namespace_, i);
        
        switch(g_base_info_get_type(info)) {
            case GI_INFO_TYPE_BOXED:
                //FIXME: GIStructInfo or GIUnionInfo
            case GI_INFO_TYPE_STRUCT:
                ParseStruct((GIStructInfo*)info, exports);
                break;
            case GI_INFO_TYPE_ENUM:
                ParseEnum((GIEnumInfo*)info, exports);
                break;
            case GI_INFO_TYPE_FLAGS:
                ParseFlags((GIEnumInfo*)info, exports);
                break;
            case GI_INFO_TYPE_OBJECT:
                GIRObject::Prepare(exports, (GIObjectInfo*)info, namespace_);
                break;
            case GI_INFO_TYPE_INTERFACE:
                ParseInterface((GIInterfaceInfo*)info, exports);
                break;
            case GI_INFO_TYPE_UNION:
                ParseUnion((GIUnionInfo*)info, exports);
                break;
            case GI_INFO_TYPE_FUNCTION:
                GIRFunction::Initialize(exports, (GIFunctionInfo*)info, namespace_);
        }
        
        
        g_base_info_unref(info);
    }
    
    // when all classes have been created we can inherit them
    GIRObject::Initialize(exports, namespace_);
    
    return exports;
}

void NamespaceLoader::ParseStruct(GIStructInfo *info, Handle<Object> &exports) {

}

void NamespaceLoader::ParseEnum(GIEnumInfo *info, Handle<Object> &exports) {
    Handle<Object> obj = Object::New();
    
    int length = g_enum_info_get_n_values(info);
    for(int i=0; i<length; i++) {
        GIValueInfo *value = g_enum_info_get_value(info, i);
        obj->Set(String::New(g_base_info_get_name(value)), Number::New(i));
    }
    exports->Set(String::New(g_base_info_get_name(info)), obj);
}

void NamespaceLoader::ParseFlags(GIEnumInfo *info, Handle<Object> &exports) {
    Handle<Object> obj = Object::New();
    
    int length = g_enum_info_get_n_values(info);
    for(int i=0; i<length; i++) {
        GIValueInfo *value = g_enum_info_get_value(info, i);
        obj->Set(String::New(g_base_info_get_name(value)), Number::New(i));
    }
    exports->Set(String::New(g_base_info_get_name(info)), obj);
}

void NamespaceLoader::ParseInterface(GIInterfaceInfo *info, Handle<Object> &exports) {

}

void NamespaceLoader::ParseUnion(GIUnionInfo *info, Handle<Object> &exports) {

}

Handle<Value> NamespaceLoader::SearchPath(const Arguments &args) {
    HandleScope scope;
    
    if(!repo) {
        repo = g_irepository_get_default();
    }
    GSList *ls = g_irepository_get_search_path();
    int l = g_slist_length(ls);
    Handle<Array> res = Array::New(l);
    
    for(int i=0; i<l; i++) {
        gpointer p = g_slist_nth_data(ls, i);
        res->Set(Number::New(i), String::New((gchar*)p));
    }
    
    return scope.Close(res);
}

Handle<Value> NamespaceLoader::IsRegistered(const Arguments &args) {
    HandleScope scope;
    
    if(!repo) {
        repo = g_irepository_get_default();
    }
    
    if(args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS();
    }
    
    String::Utf8Value namespace_(args[0]->ToString());
    char *version = NULL;
    
    if(args.Length() > 1 && args[1]->IsString()) {
        String::Utf8Value version_(args[1]);
        version = *version_;
    }
    
    return scope.Close(Boolean::New(g_irepository_is_registered(repo, *namespace_, version)));
}

Handle<Value> NamespaceLoader::GetDependencies(const Arguments &args) {
    HandleScope scope;
    
    if(!repo) {
        repo = g_irepository_get_default();
    }
    
    if(args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS();
    }
    
    String::Utf8Value namespace_(args[0]->ToString());
    
    char **versions = g_irepository_get_dependencies(repo, *namespace_);
    
    int size = 0;
    while(versions[size] != NULL) { size++; }
    
    Handle<Array> res = Array::New(size);
    for(int i=0; versions[i] != NULL; i++) {
        res->Set(i, String::New(versions[i]));
    }
    
    return scope.Close(res);
}

Handle<Value> NamespaceLoader::LoadedNamespaces(const Arguments &args) {
    HandleScope scope;
    
    if(!repo) {
        repo = g_irepository_get_default();
    }
    
    char **namespaces = g_irepository_get_loaded_namespaces(repo);
    
    int size = 0;
    while(namespaces[size] != NULL) { size++; }
    
    Handle<Array> res = Array::New(size);
    for(int i=0; namespaces[i] != NULL; i++) {
        res->Set(i, String::New(namespaces[i]));
    }
    
    return scope.Close(res);
}

Handle<Value> NamespaceLoader::GetVersion(const Arguments &args) {
    HandleScope scope;
    
    if(!repo) {
        repo = g_irepository_get_default();
    }
    
    if(args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS();
    }
    
    String::Utf8Value namespace_(args[0]->ToString());
    
    const char *version = g_irepository_get_version(repo, *namespace_);
    
    return scope.Close(String::New(version));
}

Handle<Value> NamespaceLoader::GetVersions(const Arguments &args) {
    HandleScope scope;
    
    if(!repo) {
        repo = g_irepository_get_default();
    }
    
    if(args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS();
    }
    
    String::Utf8Value namespace_(args[0]->ToString());
    
    GList *versions = g_irepository_enumerate_versions(repo, *namespace_);
    int length = g_list_length(versions);
    Handle<Array> res = Array::New(length);
    
    for(int i=0; i<length; i++) {
        res->Set(i, String::New((char*)g_list_nth_data(versions, i)));
    }
    
    return scope.Close(res);
}



}
