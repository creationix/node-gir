#include "namespace_loader.h"
#include "util.h"

#include "types/object.h"
#include "types/function_type.h"
#include "types/struct.h"

#include <string.h>

using namespace v8;

namespace gir {

GIRepository *NamespaceLoader::repo = nullptr;
std::map<char *, GITypelib*> NamespaceLoader::type_libs;

NAN_METHOD(NamespaceLoader::Load) {
    if (info.Length() < 1) {
        Nan::ThrowError("too few arguments");
    }
    if (!info[0]->IsString()) {
        Nan::ThrowError("argument has to be a string");
    }

    Local<Value> exports;
    String::Utf8Value namespace_(info[0]);

    if(info.Length() > 1 && info[1]->IsString()) {
        String::Utf8Value version(info[1]);
        exports = NamespaceLoader::LoadNamespace(*namespace_, *version);
    }
    else {
        exports = NamespaceLoader::LoadNamespace(*namespace_, nullptr);
    }

    info.GetReturnValue().Set(exports);
}

Handle<Value> NamespaceLoader::LoadNamespace(char *namespace_, char *version) {
    if (!repo) {
        repo = g_irepository_get_default();
    }

    GError *er = NULL;
    GITypelib *lib = g_irepository_require(repo, namespace_, version, (GIRepositoryLoadFlags)0, &er);
    if (!lib) {
        Nan::ThrowError(er->message);
    }

    type_libs.insert(std::make_pair(namespace_, lib));

    Handle<Value> res = BuildClasses(namespace_);
    return res;
}

Handle<Value> NamespaceLoader::BuildClasses(char *namespace_) {
    Handle<Object> exports = Nan::New<Object>();

    int length = g_irepository_get_n_infos(repo, namespace_);
    for (int i = 0; i < length; i++) {
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
    Handle<Object> obj = Nan::New<Object>();

    int length = g_enum_info_get_n_values(info);
    for(int i=0; i<length; i++) {
        GIValueInfo *value = g_enum_info_get_value(info, i);
        Nan::Set(obj, Nan::New(g_base_info_get_name(value)).ToLocalChecked(), Nan::New<Number>(g_value_info_get_value(value)));
    	g_base_info_unref(value);
    }
    Nan::Set(exports, Nan::New(g_base_info_get_name(info)).ToLocalChecked(), obj);
}

void NamespaceLoader::ParseFlags(GIEnumInfo *info, Handle<Object> &exports) {
    Handle<Object> obj = Nan::New<Object>();

    int length = g_enum_info_get_n_values(info);
    for(int i=0; i<length; i++) {
        GIValueInfo *value = g_enum_info_get_value(info, i);
        Nan::Set(obj, Nan::New(g_base_info_get_name(value)).ToLocalChecked(), Nan::New(i));
    }
    Nan::Set(exports, Nan::New(g_base_info_get_name(info)).ToLocalChecked(), obj);
}

void NamespaceLoader::ParseInterface(GIInterfaceInfo *info, Handle<Object> &exports) {

}

void NamespaceLoader::ParseUnion(GIUnionInfo *info, Handle<Object> &exports) {

}

NAN_METHOD(NamespaceLoader::SearchPath) {
    if(!repo) {
        repo = g_irepository_get_default();
    }
    GSList *ls = g_irepository_get_search_path();
    int l = g_slist_length(ls);
    Local<Array> res = Nan::New<Array>(l);

    for(int i=0; i<l; i++) {
        gpointer p = g_slist_nth_data(ls, i);
        Nan::Set(res, Nan::New(i), Nan::New((gchar*)p).ToLocalChecked());
    }

    info.GetReturnValue().Set(res);
}

}
