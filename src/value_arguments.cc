#include "value_arguments.h"
#include "util.h"

#include <vector>

using namespace v8;

namespace gir {

Handle<Value> ValueToArgs::CallFunc(GObject *obj, GIFunctionInfo *info, const Arguments &args) {
    int l = g_callable_info_get_n_args(info);
    int in_args_c = 1, out_args_c = 0;
    for(int i=0; i<l; i++) {
        GIArgInfo *arg = g_callable_info_get_arg(info, i);
        GIDirection dir = g_arg_info_get_direction(arg);
        if(dir == GI_DIRECTION_IN) {
            in_args_c++;
        }
        else if(dir == GI_DIRECTION_OUT) {
            out_args_c++;
        }
        else {
            out_args_c++;
            in_args_c++;
        }
        g_base_info_unref(arg);
    }
    
    GIArgument in_args[in_args_c];
    GIArgument out_args[out_args_c];
    
    if(args.Length()-1 < l) {
        return EXCEPTION("too few arguments");
    }
    
    int in_c = 1, out_c = 0;
    for(int i=0; i<l; i++) {
        GIArgInfo *arg = g_callable_info_get_arg(info, i);
        GIDirection dir = g_arg_info_get_direction(arg);
        if(dir == GI_DIRECTION_IN || dir == GI_DIRECTION_INOUT) {
            if(!Convert(args[i+1], &in_args[in_c], arg)) {
                return BAD_ARGS();
            }
            in_c++;
        }
        if(dir == GI_DIRECTION_OUT || dir == GI_DIRECTION_INOUT) {
            if(!Convert(args[i+1], &(out_args[out_c]), arg)) {
                return BAD_ARGS();
            }
            out_c++;
        }
        g_base_info_unref(arg);
    }
    
    in_args[0].v_pointer = obj;
    
    GError *error = NULL;
    GIArgument retval;
    
    const GIArgument *invoke_in_args = NULL;
    const GIArgument *invoke_out_args = NULL;
    if(in_c > 0) {
        invoke_in_args = (const GIArgument*)&in_args;
    }
    if(out_c > 0) {
        invoke_out_args = (const GIArgument*)&out_args;
    }

    if(!g_function_info_invoke(info, invoke_in_args, in_args_c, invoke_out_args, out_args_c, &retval, &error)) {
        return EXCEPTION(error->message);
    }
    else {
        // return retval and all out_args
        return ArgToValue(&retval, g_callable_info_get_return_type(info));
    }
    
    return Undefined();
}

bool ValueToArgs::Convert(Handle<Value> v, GIArgument *arg, GIArgInfo *info) {
    GITypeInfo *type = g_arg_info_get_type(info);
    switch(g_type_info_get_tag(type)) {
        case GI_TYPE_TAG_VOID:
            break;
        case GI_TYPE_TAG_BOOLEAN:
            if(!v->IsBoolean()) { return false; }
            arg->v_boolean = v->ToBoolean()->IsTrue(); break;
        case GI_TYPE_TAG_INT8:
            if(!v->IsNumber()) { return false; }
            arg->v_int8 = v->ToNumber()->NumberValue(); break;
        case GI_TYPE_TAG_UINT8:
            if(!v->IsNumber()) { return false; }
            arg->v_uint8 = v->ToNumber()->NumberValue(); break;
        case GI_TYPE_TAG_INT16:
            if(!v->IsNumber()) { return false; }
            arg->v_int16 = v->ToNumber()->NumberValue(); break;
        case GI_TYPE_TAG_UINT16:
            if(!v->IsNumber()) { return false; }
            arg->v_uint16 = v->ToNumber()->NumberValue(); break;
        case GI_TYPE_TAG_INT32:
            if(!v->IsNumber()) { return false; }
            arg->v_int32 = v->ToNumber()->NumberValue(); break;
        case GI_TYPE_TAG_UINT32:
            if(!v->IsNumber()) { return false; }
            arg->v_uint32 = v->ToNumber()->NumberValue(); break;
        case GI_TYPE_TAG_INT64:
            if(!v->IsNumber()) { return false; }
            arg->v_int64 = v->ToNumber()->NumberValue(); break;
        case GI_TYPE_TAG_UINT64:
            if(!v->IsNumber()) { return false; }
            arg->v_uint64 = v->ToNumber()->NumberValue(); break;
        case GI_TYPE_TAG_FLOAT:
            if(!v->IsNumber()) { return false; }
            arg->v_float = v->ToNumber()->NumberValue(); break;
        case GI_TYPE_TAG_DOUBLE:
            if(!v->IsNumber()) { return false; }
            arg->v_double = v->ToNumber()->NumberValue(); break;
        case GI_TYPE_TAG_GTYPE:
            if(!v->IsNumber()) { return false; }
            arg->v_uint = v->ToNumber()->NumberValue(); break;
        case GI_TYPE_TAG_UTF8:
            if(!v->IsString()) { return false; }
            arg->v_string = *String::Utf8Value(v->ToString()); break;
        case GI_TYPE_TAG_FILENAME:
            if(!v->IsString()) { return false; }
            arg->v_string = *String::Utf8Value(v->ToString()); break;
        case GI_TYPE_TAG_ARRAY:
            break;
        case GI_TYPE_TAG_INTERFACE:
            break;
        case GI_TYPE_TAG_GLIST:
            break;
        case GI_TYPE_TAG_GSLIST:
            break;
        case GI_TYPE_TAG_GHASH:
            break;
        case GI_TYPE_TAG_ERROR:
            break;
        case GI_TYPE_TAG_UNICHAR:
            break;
    }
    return true;
}

Handle<Value> ValueToArgs::ArgToValue(GIArgument *arg, GITypeInfo *type) {
    switch(g_type_info_get_tag(type)) {
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
            return Undefined();
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

}
