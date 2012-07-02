#include "namespace_loader.h"
#include "util.h"

#include "types/object.h"
#include "types/function.h"
#include "types/struct.h"

#include <string.h>

using namespace v8;

namespace gir {

GIRepository *NamespaceLoader::repo = NULL;
std::map<char *, GITypelib*> NamespaceLoader::type_libs;

void NamespaceLoader::Initialize(Handle<Object> target) {
    NODE_SET_METHOD(target, "load", NamespaceLoader::Load);
    NODE_SET_METHOD(target, "search_path", NamespaceLoader::SearchPath);
}

Handle<Value> NamespaceLoader::Load(const Arguments &args) {
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
                GIRStruct::Prepare(exports, (GIStructInfo*)info);
                break;
            case GI_INFO_TYPE_ENUM:
                ParseEnum((GIEnumInfo*)info, exports);
                break;
            case GI_INFO_TYPE_FLAGS:
                ParseFlags((GIEnumInfo*)info, exports);
                break;
            case GI_INFO_TYPE_OBJECT:
                GIRObject::Prepare(exports, (GIObjectInfo*)info);
                break;
            case GI_INFO_TYPE_INTERFACE:
                ParseInterface((GIInterfaceInfo*)info, exports);
                break;
            case GI_INFO_TYPE_UNION:
                ParseUnion((GIUnionInfo*)info, exports);
                break;
            case GI_INFO_TYPE_FUNCTION:
                GIRFunction::Initialize(exports, (GIFunctionInfo*)info);
                break;
            case GI_INFO_TYPE_INVALID:
            case GI_INFO_TYPE_CALLBACK:
            case GI_INFO_TYPE_CONSTANT:
            case GI_INFO_TYPE_INVALID_0:
            case GI_INFO_TYPE_VALUE:
            case GI_INFO_TYPE_SIGNAL:
            case GI_INFO_TYPE_VFUNC:
            case GI_INFO_TYPE_PROPERTY:
            case GI_INFO_TYPE_FIELD:
            case GI_INFO_TYPE_ARG:
            case GI_INFO_TYPE_TYPE:
            case GI_INFO_TYPE_UNRESOLVED:
                // Do nothing
                break;
        }
          
        g_base_info_unref(info);
    }
    
    // when all classes have been created we can inherit them
    GIRObject::Initialize(exports, namespace_);
    GIRStruct::Initialize(exports, namespace_);
    
    return exports;
}

void NamespaceLoader::ParseStruct(GIStructInfo *info, Handle<Object> &exports) {

}

void NamespaceLoader::ParseEnum(GIEnumInfo *info, Handle<Object> &exports) {
    Handle<Object> obj = Object::New();
    
    int length = g_enum_info_get_n_values(info);
    for(int i=0; i<length; i++) {
        GIValueInfo *value = g_enum_info_get_value(info, i);
        obj->Set(String::New(g_base_info_get_name(value)), Number::New(g_value_info_get_value(value)));
	g_base_info_unref(value);
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



}
