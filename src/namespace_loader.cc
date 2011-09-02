#include "namespace_loader.h"
#include "util.h"
#include "object.h"

using namespace v8;

namespace gir {

GIRepository *NamespaceLoader::repo = NULL;
std::map<char *, GITypelib*> NamespaceLoader::type_libs;

void NamespaceLoader::Initialize(Handle<Object> target) {
    GIR_SET_METHOD(target, "load", NamespaceLoader::Load);
}

Handle<Value> NamespaceLoader::Load(const Arguments &args) {
    HandleScope scope;
    
    if(args.Length() < 1) {
        EXCEPTION("too few arguments");
    }
    if(!args[0]->IsString()) {
        EXCEPTION("argument has to be a string");
    }
    
    String::Utf8Value namespace_(args[0]);
    Handle<Value> exports = NamespaceLoader::LoadNamespace(*namespace_);
    
    return scope.Close(exports);
}

Handle<Value> NamespaceLoader::LoadNamespace(char *namespace_) {
    if(!repo) {
        repo = g_irepository_get_default();
    }
    
    GError *er = NULL;
    GITypelib *lib = g_irepository_require(repo, namespace_, NULL, (GIRepositoryLoadFlags)0, &er);
    if(!lib) {
        return EXCEPTION(er->message);
    }
    type_libs.insert(std::make_pair(namespace_, lib));
    
    return BuildClasses(namespace_);
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
                ParseObject((GIObjectInfo*)info, exports);
                break;
            case GI_INFO_TYPE_INTERFACE:
                ParseInterface((GIInterfaceInfo*)info, exports);
                break;
            case GI_INFO_TYPE_UNION:
                ParseUnion((GIUnionInfo*)info, exports);
                break;
        }
        
        
        g_base_info_unref(info);
    }
    
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

void NamespaceLoader::ParseObject(GIObjectInfo *info, Handle<Object> &exports) {
    //printf("%s\n", g_type_name(g_registered_type_info_get_g_type(info)) );
    GIRObject::Initialize(exports, info);
}

void NamespaceLoader::ParseInterface(GIInterfaceInfo *info, Handle<Object> &exports) {

}

void NamespaceLoader::ParseUnion(GIUnionInfo *info, Handle<Object> &exports) {

}



}
