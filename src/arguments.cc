#include "arguments.h"
#include "util.h"
#include "values.h"

#include "types/object.h"
#include <string.h>

#include <vector>

using namespace v8;

namespace gir {

bool Args::ToGType(Handle<Value> v, GIArgument *arg, GIArgInfo *info, GITypeInfo *type_info, bool out) {
    GITypeInfo *type = type_info;
    if (info != NULL) 
        type = g_arg_info_get_type(info);
    GITypeTag tag = ReplaceGType(g_type_info_get_tag(type));

    // nullify string so it be freed safely later 
    arg->v_string = NULL;

    if (out == TRUE)
        return true;

    if( ( v == Null() || v == Undefined() ) 
		    && (g_arg_info_may_be_null(info) 
			    || tag == GI_TYPE_TAG_VOID)) {
        arg->v_pointer = NULL;
        return true;
    }
    if(tag == GI_TYPE_TAG_BOOLEAN) {
        arg->v_boolean = v->ToBoolean()->Value();
        return true;
    }
    if(tag == GI_TYPE_TAG_INT8) {
        arg->v_int8 = v->ToNumber()->NumberValue();
        return true;
    }
    if(tag == GI_TYPE_TAG_UINT8) {
        arg->v_uint8 = v->ToNumber()->NumberValue();
        return true;
    }
    if(tag == GI_TYPE_TAG_INT16) {
        arg->v_int16 = v->ToNumber()->NumberValue();
        return true;
    }
    if(tag == GI_TYPE_TAG_UINT16) {
        arg->v_uint16 = v->ToNumber()->NumberValue();
        return true;
    }
    if(tag == GI_TYPE_TAG_INT32) {
        arg->v_int32 = v->ToInt32()->Value();
        return true;
    }
    if(tag == GI_TYPE_TAG_UINT32) {
        arg->v_uint32 = v->ToUint32()->Value();
        return true;
    }
    if(tag == GI_TYPE_TAG_INT64) {
        arg->v_int64 = v->ToInteger()->Value();
        return true;
    }
    if(tag == GI_TYPE_TAG_UINT64) {
        arg->v_uint64 = v->ToInteger()->Value();
        return true;
    }
    if(tag == GI_TYPE_TAG_FLOAT) {
        arg->v_float = v->ToNumber()->Value();
        return true;
    }
    if(tag == GI_TYPE_TAG_DOUBLE) {
        arg->v_double = v->ToNumber()->Value();
        return true;
    }
    if(tag == GI_TYPE_TAG_UTF8 || tag == GI_TYPE_TAG_FILENAME) {
        String::Utf8Value v8str(v->ToString());
        arg->v_string = g_strdup(*v8str);
        return true;
    }
    if(tag == GI_TYPE_TAG_GLIST) {
        if(!v->IsArray()) { return false; }
        //GList *list = NULL;
        //ArrayToGList(v, info, &list); // FIXME!!!
        return false;
    }
    if(tag == GI_TYPE_TAG_GSLIST) {
        if(!v->IsArray()) { return false; }
        //GSList *list = NULL;
        //ArrayToGList(v, info, &list); // FIXME!!!
        return false;
    }
    if(tag == GI_TYPE_TAG_ARRAY) {
        if(!v->IsArray()) {
            if (v->IsString()) {
                String::Utf8Value _str(v->ToString());
                arg->v_pointer = (gpointer *) g_strdup(*_str);
                return true;
            }
            return false; 
        }
        
        GIArrayType arr_type = g_type_info_get_array_type(info);
        
        if(arr_type == GI_ARRAY_TYPE_C) {
        
        }
        else if(arr_type == GI_ARRAY_TYPE_ARRAY) {
        
        }
        else if(arr_type == GI_ARRAY_TYPE_PTR_ARRAY) {
        
        }
        else if(arr_type == GI_ARRAY_TYPE_BYTE_ARRAY) {
            
        }
        /*
        int l = g_type_info_get_array_length(info);
        for(int i=0; i<l; i++) {
            
        }*/
        return false;
    }
    if(tag == GI_TYPE_TAG_GHASH) {
        if(!v->IsObject()) { return false; }
        
        GITypeInfo *key_param_info, *val_param_info;
        //GHashTable *ghash;

        key_param_info = g_type_info_get_param_type(info, 0);
        g_assert(key_param_info != NULL);
        val_param_info = g_type_info_get_param_type(info, 1);
        g_assert(val_param_info != NULL);
        
        // TODO: implement

        g_base_info_unref((GIBaseInfo*) key_param_info);
        g_base_info_unref((GIBaseInfo*) val_param_info);
        
        return false;
    }
    if(tag == GI_TYPE_TAG_INTERFACE) {
        GIBaseInfo *interface_info = g_type_info_get_interface(type);
        g_assert(interface_info != NULL);
        GIInfoType interface_type = g_base_info_get_type(interface_info);
        
        GType gtype;
        switch(interface_type) {
            case GI_INFO_TYPE_STRUCT:
            case GI_INFO_TYPE_ENUM:
            case GI_INFO_TYPE_OBJECT:
            case GI_INFO_TYPE_INTERFACE:
            case GI_INFO_TYPE_UNION:
            case GI_INFO_TYPE_BOXED:
                gtype = g_registered_type_info_get_g_type
                    ((GIRegisteredTypeInfo*)interface_info);
                break;
            case GI_INFO_TYPE_VALUE:
                gtype = G_TYPE_VALUE;
                break;

            default:
                gtype = G_TYPE_NONE;
                break;
        }

        if(g_type_is_a(gtype, G_TYPE_OBJECT)) {
            if(!v->IsObject()) { return false; }
            GIRObject *gir_object = node::ObjectWrap::Unwrap<GIRObject>(v->ToObject());
            arg->v_pointer = gir_object->obj;
            return true;
        }
        if(g_type_is_a(gtype, G_TYPE_VALUE)) {
            GValue gvalue = {0,};
            if(!GIRValue::ToGValue(v, G_TYPE_INVALID, &gvalue)) {
                return false;
            }
            //FIXME I've to free this somewhere
            arg->v_pointer = g_boxed_copy(G_TYPE_VALUE, &gvalue);
            g_value_unset(&gvalue);
            return true;
        }
    }
    
    return false;
}

Handle<Value> Args::FromGTypeArray(GIArgument *arg, GITypeInfo *type, int array_length) {

    GITypeInfo *param_info = g_type_info_get_param_type(type, 0);
    //bool is_zero_terminated = g_type_info_is_zero_terminated(param_info);
    GITypeTag param_tag = g_type_info_get_tag(param_info);

    //g_base_info_unref(param_info);

    int i = 0;
    v8::Local<v8::Array> arr;
    GIBaseInfo *interface_info = NULL;

    switch(param_tag) {
        case GI_TYPE_TAG_UINT8:
            if (arg->v_pointer == NULL)
                return String::New("");
            // TODO, copy bytes to array
            // http://groups.google.com/group/v8-users/browse_thread/thread/8c5177923675749e?pli=1
            return String::New((char *)arg->v_pointer, array_length); 
            
        case GI_TYPE_TAG_GTYPE:
            if (arg->v_pointer == NULL)
                return v8::Array::New(0);
            arr = v8::Array::New(array_length);
            for (i = 0; i < array_length; i++) { 
                arr->Set(i, v8::Integer::New((int)GPOINTER_TO_INT((((gpointer*)arg->v_pointer)[i]))));
            }
            return arr;

        case GI_TYPE_TAG_INTERFACE:
            if (arg->v_pointer == NULL)
                return v8::Array::New(0);
            arr = v8::Array::New(array_length);
            interface_info = g_type_info_get_interface(param_info);

            for (i = 0; i < array_length; i++) {                
                GObject *o = (GObject*)((gpointer*)arg->v_pointer)[i];
                arr->Set(i, GIRObject::New(o, interface_info));
            }
            //g_base_info_unref(interface_info); // FIXME
            return arr;

        default:
            gchar *exc_msg = g_strdup_printf("Converting array of '%s' is not supported", g_type_tag_to_string(param_tag));
            return EXCEPTION(exc_msg); 
    }
}

Handle<Value> Args::FromGType(GIArgument *arg, GITypeInfo *type, int array_length) {
    GITypeTag tag = g_type_info_get_tag(type);

    if(tag == GI_TYPE_TAG_INTERFACE) {
        GIBaseInfo *interface_info = g_type_info_get_interface(type);
        g_assert(interface_info != NULL);
        GIInfoType interface_type = g_base_info_get_type(interface_info);
        
        if(interface_type == GI_INFO_TYPE_OBJECT) {
            return GIRObject::New(G_OBJECT(arg->v_pointer), interface_info);
        }
    }
    
    if(tag == GI_TYPE_TAG_INTERFACE) {
        GIBaseInfo *interface_info = g_type_info_get_interface(type);
        g_assert(interface_info != NULL);
        GIInfoType interface_type = g_base_info_get_type(interface_info);
        
        GType gtype;
        switch(interface_type) {
            case GI_INFO_TYPE_STRUCT:
            case GI_INFO_TYPE_ENUM:
            case GI_INFO_TYPE_OBJECT:
            case GI_INFO_TYPE_INTERFACE:
            case GI_INFO_TYPE_UNION:
            case GI_INFO_TYPE_BOXED:
                gtype = g_registered_type_info_get_g_type
                    ((GIRegisteredTypeInfo*)interface_info);
                break;
            case GI_INFO_TYPE_VALUE:
                gtype = G_TYPE_VALUE;
                break;

            default:
                gtype = G_TYPE_NONE;
                break;
        }
        
        if(g_type_is_a(gtype, G_TYPE_OBJECT)) {
            GObject *o = G_OBJECT(arg->v_pointer);
            return GIRObject::New(o, G_OBJECT_TYPE(o));
        }
        if(g_type_is_a(gtype, G_TYPE_VALUE)) {
            GIRValue::FromGValue((GValue*)arg->v_pointer, NULL);
        }
    }
    
    switch(tag) {
        case GI_TYPE_TAG_VOID:
            return Undefined();
        case GI_TYPE_TAG_BOOLEAN:
            return Boolean::New(arg->v_boolean);
        case GI_TYPE_TAG_INT8:
            return Integer::New(arg->v_int8);
        case GI_TYPE_TAG_UINT8:
            return Integer::NewFromUnsigned(arg->v_uint8);
        case GI_TYPE_TAG_INT16:
            return Integer::New(arg->v_int16);
        case GI_TYPE_TAG_UINT16:
            return Integer::NewFromUnsigned(arg->v_uint16);
        case GI_TYPE_TAG_INT32:
            return Integer::New(arg->v_int32);
        case GI_TYPE_TAG_UINT32:
            return Integer::NewFromUnsigned(arg->v_uint32);
        case GI_TYPE_TAG_INT64:
            return Integer::New(arg->v_int64);
        case GI_TYPE_TAG_UINT64:
            return Integer::NewFromUnsigned(arg->v_uint64);
        case GI_TYPE_TAG_FLOAT:
            return Number::New(arg->v_float);
        case GI_TYPE_TAG_DOUBLE:
            return Number::New(arg->v_double);
        case GI_TYPE_TAG_GTYPE:
            return Integer::NewFromUnsigned(arg->v_uint);
        case GI_TYPE_TAG_UTF8:
            return String::New(arg->v_string);
        case GI_TYPE_TAG_FILENAME:
            return String::New(arg->v_string);
        case GI_TYPE_TAG_ARRAY:
            return Args::FromGTypeArray(arg, type, array_length);
        case GI_TYPE_TAG_INTERFACE:
            return Undefined();
        case GI_TYPE_TAG_GLIST:
            return Undefined();
        case GI_TYPE_TAG_GSLIST:
            return Undefined();
        case GI_TYPE_TAG_GHASH:
            return Undefined();
        case GI_TYPE_TAG_ERROR:
            return Undefined();
        case GI_TYPE_TAG_UNICHAR:
            return Undefined();
        default:
            return Undefined();
    }
}

GITypeTag Args::ReplaceGType(GITypeTag type) {
    if(type == GI_TYPE_TAG_GTYPE) {
        switch (sizeof(GType)) {
        case 1: return GI_TYPE_TAG_UINT8;
        case 2: return GI_TYPE_TAG_UINT16;
        case 4: return GI_TYPE_TAG_UINT32;
        case 8: return GI_TYPE_TAG_UINT64;
        default: g_assert_not_reached();
        }
    }
    return type;
}

bool Args::ArrayToGList(Handle<Array> arr, GIArgInfo *info, GList **list_p) {
    GList *list = NULL;

    int l = arr->Length();
    for(int i=0; i<l; i++) {
        GIArgument arg = {0,};
        if(!Args::ToGType(arr->Get(Number::New(i)), &arg, g_type_info_get_param_type(info, 0), NULL, FALSE)) {
            return false;
        }
        list = g_list_prepend(list, arg.v_pointer);
    }

    list = g_list_reverse(list);
    *list_p = list;

    return true;
}

bool Args::ArrayToGList(Handle<Array> arr, GIArgInfo *info, GSList **slist_p) {
    GSList *slist = NULL;

    int l = arr->Length();
    for(int i=0; i<l; i++) {
        GIArgument arg = {0,};
        if(!Args::ToGType(arr->Get(Number::New(i)), &arg, g_type_info_get_param_type(info, 0), NULL, FALSE)) {
            return false;
        }
        slist = g_slist_prepend(slist, arg.v_pointer);
    }

    slist = g_slist_reverse(slist);
    *slist_p = slist;

    return true;
}


}
