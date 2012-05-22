#include "values.h"
#include "util.h"

#include "types/object.h"

using namespace v8;

namespace gir {

Handle<Value> GIRValue::FromGValue(GValue *v) {
    GType type = G_VALUE_TYPE(v);
    Handle<Value> value = Undefined();
    
    if(g_type_is_a(type, G_TYPE_CHAR)) {
        char *str = new char[2];
        str[0] = g_value_get_char(v);
        str[1] = '\0';
        value = String::New(str);
        delete[] str;
    }
    else if(g_type_is_a(type, G_TYPE_UCHAR)) {
        char *str = new char[2];
        str[0] = g_value_get_uchar(v);
        str[1] = '\0';
        value = String::New(str);
        delete[] str;
    }
    else if(g_type_is_a(type, G_TYPE_BOOLEAN)) {
        value = Boolean::New(g_value_get_boolean(v));
    }
    else if(g_type_is_a(type, G_TYPE_INT)) {
        value = Number::New(g_value_get_int(v));
    }
    else if(g_type_is_a(type, G_TYPE_UINT) || g_type_is_a(type, G_TYPE_GTYPE)) {
        value = Number::New(g_value_get_uint(v));
    }
    else if(g_type_is_a(type, G_TYPE_LONG)) {
        value = Number::New(g_value_get_long(v));
    }
    else if(g_type_is_a(type, G_TYPE_ULONG)) {
        value = Number::New(g_value_get_ulong(v));
    }
    else if(g_type_is_a(type, G_TYPE_INT64)) {
        value = Number::New(g_value_get_int64(v));
    }
    else if(g_type_is_a(type, G_TYPE_UINT64)) {
        value = Number::New(g_value_get_uint64(v));
    }
    else if(g_type_is_a(type, G_TYPE_ENUM)) {
        value = Number::New(g_value_get_enum(v));
    }
    else if(g_type_is_a(type, G_TYPE_FLAGS)) {
        value = Number::New(g_value_get_flags(v));
    }
    else if(g_type_is_a(type, G_TYPE_FLOAT)) {
        value = Number::New(g_value_get_float(v));
    }
    else if(g_type_is_a(type, G_TYPE_DOUBLE)) {
        value = Number::New(g_value_get_double(v));
    }
    else if(g_type_is_a(type, G_TYPE_STRING)) {
        value = String::New(g_value_get_string(v));
    }
    else if(g_type_is_a(type, G_TYPE_POINTER)) {
    
    }
    else if(g_type_is_a(type, G_TYPE_BOXED)) {
    
    }
    else if(g_type_is_a(type, G_TYPE_PARAM)) {
    
    }
    else if(g_type_is_a(type, G_TYPE_OBJECT)) {
        value = GIRObject::New(G_OBJECT(g_value_get_object(v)), type);
    }
    return value;
}

bool GIRValue::ToGValue(Handle<Value> value, GType type, GValue *v) {
    if(type == G_TYPE_INVALID || type == 0) {
        type = GIRValue::GuessType(value);
    }
    if(type == 0) {
        return false;
    }
    
    g_value_init(v, type);
    
    if(g_type_is_a(type, G_TYPE_INTERFACE)) {
        if(g_type_is_a(type, G_TYPE_OBJECT)) {
            if(/* has instance */ true) {
                g_value_set_object(v, node::ObjectWrap::Unwrap<GIRObject>(value->ToObject())->obj);
                return true;
            }
            else { return false; }
        }
    }
    else if(g_type_is_a(type, G_TYPE_CHAR)) {
        if(value->IsString()) {
            String::Utf8Value str(value);
            g_value_set_char(v, (*str)[0]);
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_UCHAR)) {
        if(value->IsString()) {
            String::Utf8Value str(value);
            g_value_set_uchar(v, (*str)[0]);
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_BOOLEAN)) {
        if(value->IsBoolean()) {
            g_value_set_boolean(v, value->ToBoolean()->IsTrue());
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_INT)) {
        if(value->IsNumber()) {
            g_value_set_int(v, value->NumberValue());
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_UINT) || g_type_is_a(type, G_TYPE_GTYPE)) {
        if(value->IsNumber()) {
            g_value_set_uint(v, value->NumberValue());
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_LONG)) {
        if(value->IsNumber()) {
            g_value_set_long(v, value->NumberValue());
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_ULONG)) {
        if(value->IsNumber()) {
            g_value_set_ulong(v, value->NumberValue());
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_INT64)) {
        if(value->IsNumber()) {
            g_value_set_int64(v, value->NumberValue());
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_UINT64)) {
        if(value->IsNumber()) {
            g_value_set_uint64(v, value->NumberValue());
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_ENUM)) {
        if(value->IsNumber()) {
            g_value_set_enum(v, value->NumberValue());
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_FLAGS)) {
        if(value->IsNumber()) {
            g_value_set_flags(v, value->NumberValue());
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_FLOAT)) {
        if(value->IsNumber()) {
            g_value_set_float(v, value->NumberValue());
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_DOUBLE)) {
        if(value->IsNumber()) {
            // Ugly, but seems to do the trick. 
            // Feel free to rewrite.
            GValue fval = {0, };
            g_value_init(&fval, G_TYPE_STRING);
            g_value_set_string(&fval, g_strdup_printf("%.2f", value->NumberValue()));
            g_value_transform(&fval, v);
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_STRING)) {
        if(value->IsString()) {
            g_value_set_string(v, *String::Utf8Value(value->ToString()) );
            return true;
        }
        else { return false; }
    }
    else if(g_type_is_a(type, G_TYPE_POINTER)) {
        
    }
    else if(g_type_is_a(type, G_TYPE_BOXED)) {
    
    }
    else if(g_type_is_a(type, G_TYPE_PARAM)) {
    
    }
    else if(g_type_is_a(type, G_TYPE_OBJECT)) {
    
    }
    return false;
}

GType GIRValue::GuessType(Handle<Value> value) {
    if(value->IsString()) {
        return G_TYPE_STRING;
    }
    else if(value->IsArray()) {
        return G_TYPE_ARRAY;
    }
    else if(value->IsBoolean()) {
        return G_TYPE_BOOLEAN;
    }
    else if(value->IsInt32()) {
        return G_TYPE_INT;
    }
    else if(value->IsUint32()) {
        return G_TYPE_UINT;
    }
    else if(value->IsNumber()) {
        return G_TYPE_DOUBLE;
    }
    return G_TYPE_INVALID;
}

}
