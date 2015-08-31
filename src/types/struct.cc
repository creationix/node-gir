#include <iostream>

#include "struct.h"
#include "function_type.h"
#include "../util.h"
#include "../function.h"
#include "../values.h"
#include "../arguments.h"

#include <string.h>
#include <node.h>
#include <nan.h>

using namespace v8;
using namespace std;

namespace gir {

std::vector<StructFunctionTemplate> GIRStruct::templates;
std::vector<StructData> GIRStruct::instances;
static Persistent<String> emit_symbol;

GIRStruct::GIRStruct(GIStructInfo *info) 
{ 
    //c_structure = g_try_malloc0(g_struct_info_get_size ((GIStructInfo*)info));
}

Handle<Value> GIRStruct::New(gpointer c_structure, GIStructInfo *info) 
{
    Handle<Value> res = GetStructure(c_structure);
    if (res != Nan::Null()) {
        return res;
    }

    std::vector<StructFunctionTemplate>::iterator it;
    
    for (it = templates.begin(); it != templates.end(); ++it) {
        if (g_base_info_equal(info, it->info)) {
            res = it->function->GetFunction()->NewInstance();
            break;
        }
    }
  
    if (!res.IsEmpty()) {
        GIRStruct *s = ObjectWrap::Unwrap<GIRStruct>(res->ToObject()); 
        s->info = info;
        if (s->c_structure)
            g_free(s->c_structure);
        s->c_structure = c_structure;
    }

    return res;
}

NAN_METHOD(GIRStruct::New) 
{
    String::Utf8Value className(info.Callee()->GetName());
    debug_printf ("Struct constructor '%s' \n", *className);
    std::vector<StructFunctionTemplate>::iterator it;

    GIObjectInfo *gobjinfo = nullptr;
    for (it = templates.begin(); it != templates.end(); ++it) {
        if (strcmp(it->type_name, *className) == 0) {
            gobjinfo = it->info;
            break;
        }
    }

    if (gobjinfo == nullptr || !GI_IS_STRUCT_INFO(gobjinfo)) {
        Nan::ThrowError("Missed introspection structure info");
    }
   
    GIBaseInfo *func  = (GIBaseInfo*) g_struct_info_find_method(gobjinfo, "new");
    if (func == nullptr) {
        Nan::ThrowError("Missed introspection structure constructor info");
    }
   
    GIArgument retval;
    GITypeInfo *returned_type_info;
    gint returned_array_length;
    Func::CallAndGetPtr(nullptr, func, info, TRUE, &retval, &returned_type_info, &returned_array_length);
    
    if (returned_type_info != nullptr)
        g_base_info_unref(returned_type_info);

    GIRStruct *obj = new GIRStruct(gobjinfo);

    /* Set underlying C structure */
    obj->c_structure = (gpointer) retval.v_pointer;

    obj->Wrap(info.This());
    PushInstance(obj, info.This());
    obj->info = gobjinfo;

    info.GetReturnValue().Set(info.This());
}

GIFieldInfo *_find_structure_member(GIStructInfo *info, const gchar *name)
{
    gint n_fields = g_struct_info_get_n_fields(info);
    for(int i = 0; i < n_fields; i++) {
        GIFieldInfo *field = g_struct_info_get_field(info, i);
        if (g_str_equal(g_base_info_get_name(field), name))
            return field;
        g_base_info_unref(field);
    }
    return nullptr;
}

NAN_PROPERTY_GETTER(FieldGetHandler) 
{
    String::Utf8Value _name(property);
    v8::Handle<v8::External> info_ptr = v8::Handle<v8::External>::Cast(info.Data());
    GIBaseInfo *base_info  = (GIBaseInfo*) info_ptr->Value();
    GIFieldInfo *field_info = nullptr;
    if (base_info != nullptr)
        field_info = _find_structure_member(base_info, *_name);

    if (field_info) {       
        if (!(g_field_info_get_flags(field_info) & GI_FIELD_IS_READABLE)) {
            // field is not readable
            Nan::ThrowError("member is not readable");
        }        
        GIRStruct *that = Nan::ObjectWrap::Unwrap<GIRStruct>(info.This()->ToObject());
        GIArgument arg = {0, };
        Local<Value> res;
        debug_printf("GetHandler [%p] (Get structure member) '%s.%s' \n", that->c_structure, g_base_info_get_name(base_info), *_name);
        GITypeInfo *type_info = g_field_info_get_type(field_info);
        if (g_field_info_get_field(field_info, that->c_structure, &arg) == TRUE) {
            res = Args::FromGType(&arg, type_info, -1);
        } else {
            res = Nan::Undefined();
        }
    
        // TODO free arg.v_string
        g_base_info_unref(type_info);
        g_base_info_unref(field_info);
        
        info.GetReturnValue().Set(res);
    }

    // Fallback to defaults
    info.GetReturnValue().Set(info.This()->GetPrototype()->ToObject()->Get(property));
}

NAN_PROPERTY_QUERY(FieldQueryHandler) 
{
    String::Utf8Value _name(property);
    debug_printf("QUERY HANDLER '%s' \n", *_name);
    info.GetReturnValue().Set(Nan::New<v8::Integer>(0));
}

NAN_PROPERTY_SETTER(FieldSetHandler) 
{
    String::Utf8Value _name(property);

    v8::Handle<v8::External> info_ptr = v8::Handle<v8::External>::Cast(info.Data());
    GIBaseInfo *base_info  = (GIBaseInfo*) info_ptr->Value();
    GIFieldInfo *field_info = nullptr;
    if (base_info != nullptr)
        field_info = _find_structure_member(base_info, *_name);

    if (field_info) {       
        if (!(g_field_info_get_flags(field_info) & GI_FIELD_IS_WRITABLE)) { 
            // field is not writable
            Nan::ThrowError("member is not writable");
        }
        
        GIRStruct *that = Nan::ObjectWrap::Unwrap<GIRStruct>(info.This()->ToObject());
        debug_printf("SetHandler [%p] (Set structure member) '%s.%s' \n", that->c_structure, g_base_info_get_name(base_info), *_name);
        GIArgument arg = {0, };
        Handle<Value> res;
        GITypeInfo *type_info = g_field_info_get_type(field_info);
        // FIXME, add TypeInfo argument when ArgInfo is nullptr
        bool is_set = Args::ToGType(value, &arg, nullptr, type_info, false);
        if (g_field_info_set_field(field_info, that->c_structure, &arg) == false) {
            Nan::ThrowError("Failed to set structure's field");
        } 
 
        /*GIArgument ar = {0, };
        if (g_field_info_get_field(field_info, that->c_structure, &ar) == TRUE) {
            Handle<Value> ret = Args::FromGType(&arg, type_info, -1);
        } */
        // TODO, free arg.v_string 
        g_base_info_unref(type_info);
        g_base_info_unref(field_info);

        info.GetReturnValue().Set(Nan::New<v8::Boolean>(is_set));
    }

    // Fallback to defaults
    info.GetReturnValue().Set(Nan::New<v8::Boolean>(info.This()->GetPrototype()->ToObject()->Set(property, value)));
}

void GIRStruct::Prepare(Handle<Object> target, GIStructInfo *info) 
{
    char *name = (char*)g_base_info_get_name(info);
    const char *namespace_ = g_base_info_get_namespace(info);
    g_base_info_ref(info);

    // Ignore, if this is gtype (object class) struct.
    if (g_struct_info_is_gtype_struct(info))
        return;

    // Ignore all Private and IFace structures
    if (g_str_has_suffix(name, "Private")
            || g_str_has_suffix(name, "IFace"))
        return;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->SetClassName(Nan::New<String>(name).ToLocalChecked());
    
    StructFunctionTemplate oft;
    oft.type_name = name;
    oft.info = info;
    oft.function = t;
    oft.type = g_registered_type_info_get_g_type(info);
    oft.namespace_ = (char*)namespace_;
    
    templates.push_back(oft);

    // Create instance template
    v8::Local<v8::ObjectTemplate> instance_t = t->InstanceTemplate();
    instance_t->SetInternalFieldCount(1);
    // Create external to hold GIBaseInfo and set it
    v8::Handle<v8::External> info_handle = Nan::New<v8::External>((void*)g_base_info_ref(info));
    // Set fields handlers
    SetNamedPropertyHandler(instance_t, FieldGetHandler, FieldSetHandler, FieldQueryHandler, 0, 0, info_handle);

    RegisterMethods(target, info, namespace_, t);
}

void GIRStruct::Initialize(Handle<Object> target, char *namespace_) 
{
    // this gets called when all structures have been initialized
    std::vector<StructFunctionTemplate>::iterator it;
    std::vector<StructFunctionTemplate>::iterator temp;

    for (it = templates.begin(); it != templates.end(); ++it) {
        if (strcmp(it->namespace_, namespace_) == 0) { 
            target->Set(Nan::New<String>(g_base_info_get_name(it->info)).ToLocalChecked(), it->function->GetFunction());
        }
    } 
}

void GIRStruct::PushInstance(GIRStruct *obj, Handle<Value> value) 
{
    Local<Object> p_value = value->ToObject();
    obj->MakeWeak();
    
    StructData data;
    data.gir_structure = obj;
    data.instance = p_value;
    instances.push_back(data);
}

Handle<Value> GIRStruct::GetStructure(gpointer c_structure) 
{
    std::vector<StructData>::iterator it;
    for (it = instances.begin(); it != instances.end(); it++) {
        if (it->gir_structure && it->gir_structure->c_structure && it->gir_structure->c_structure == c_structure) {
            return it->instance;
        }
    }
    return Nan::Null();
}

NAN_METHOD(GIRStruct::CallMethod) 
{
    String::Utf8Value fname(info.Callee()->GetName());
    GIRStruct *that = Nan::ObjectWrap::Unwrap<GIRStruct>(info.This()->ToObject());
    if (!GI_IS_STRUCT_INFO(that->info)) {
	   Nan::ThrowError("Missed structure info to call method");
    }
    GIFunctionInfo *func = g_struct_info_find_method(that->info, *fname);
    debug_printf("Call Method: '%s' [%p] \n", *fname, func);
    if (func) {
        debug_printf("\t Call symbol: '%s' \n", g_function_info_get_symbol(func));
        info.GetReturnValue().Set(Func::Call((GObject *)that->c_structure, func, info, TRUE));
    }
    else {
        Nan::ThrowError("no such method");
    }
    info.GetReturnValue().SetUndefined();
}

Handle<Object> GIRStruct::PropertyList(GIObjectInfo *info) 
{
    Handle<Object> list = Nan::New<Object>();
    bool first = true;
    int gcounter = 0;
    g_base_info_ref(info);
    
    while (true) {
        if (!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if (!parent) {
                return list;
            }
            if (strcmp(g_base_info_get_name(parent), g_base_info_get_name(info)) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_properties(info);
        for (int i=0; i<l; i++) {
            GIPropertyInfo *prop = g_object_info_get_property(info, i);
            list->Set(Nan::New<Number>(i+gcounter), Nan::New<String>(g_base_info_get_name(prop)).ToLocalChecked());
            g_base_info_unref(prop);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRStruct::MethodList(GIObjectInfo *info) 
{
    Handle<Object> list = Nan::New<Object>();
    bool first = true;
    int gcounter = 0;
    g_base_info_ref(info);
    
    while (true) {
        if (!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if (!parent) {
                return list;
            }
            if (strcmp( g_base_info_get_name(parent), g_base_info_get_name(info) ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_methods(info);
        for (int i=0; i<l; i++) {
            GIFunctionInfo *func = g_object_info_get_method(info, i);
            list->Set(Nan::New<Number>(i+gcounter), Nan::New<String>(g_base_info_get_name(func)).ToLocalChecked());
            g_base_info_unref(func);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

void GIRStruct::RegisterMethods(Handle<Object> target, GIStructInfo *info, const char *namespace_, Handle<FunctionTemplate> t) 
{   
    int l = g_struct_info_get_n_methods(info);
    for (int i=0; i<l; i++) {
        GIFunctionInfo *func = g_struct_info_get_method(info, i);
        const char *func_name = g_base_info_get_name(func);
        GIFunctionInfoFlags func_flag = g_function_info_get_flags(func);
        // Determine if method is static one.
        // If given function is neither method nor constructor, it's most likely static method.
        // In such case, do not set prototype method.
        /*if (func_flag & GI_FUNCTION_IS_METHOD) { 
            NODE_SET_PROTOTYPE_METHOD(t, func_name, CallMethod);
            printf ("REGISTER STRUCT METHOD '%s' \n", g_function_info_get_symbol (func));*/
        if ((func_flag & GI_FUNCTION_IS_CONSTRUCTOR)) {
            // Create new function
            Local< Function > callback_func = Nan::New<v8::FunctionTemplate>(Func::CallStaticMethod)->GetFunction();
            // Set name
            callback_func->SetName(Nan::New<String>(func_name).ToLocalChecked());
            // Create external to hold GIBaseInfo and set it
            v8::Handle<v8::External> info_ptr = Nan::New<v8::External>((void*)g_base_info_ref(func));
            callback_func->SetHiddenValue(Nan::New<String>("GIInfo").ToLocalChecked(), info_ptr);
            // Set v8 function
            t->Set(Nan::New<String>(func_name).ToLocalChecked(), callback_func);
            //printf ("REGISTER STRUCT CTR '%s' \n", g_function_info_get_symbol (func));
        } else {
            Nan::SetPrototypeMethod(t, func_name, CallMethod);
            //printf ("REGISTER STRUCT METHOD '%s' \n", g_function_info_get_symbol (func));
        }
        g_base_info_unref(func);
    }
}

Handle<Object> GIRStruct::FieldList(GIObjectInfo *info) 
{
    Handle<Object> list = Nan::New<Object>();
    bool first = true;
    int gcounter = 0;
    g_base_info_ref(info);
    
    while (true) {
        if (!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if (!parent) {
                return list;
            }
            if (strcmp( g_base_info_get_name(parent), g_base_info_get_name(info) ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_fields(info);
        for (int i=0; i<l; i++) {
            GIFieldInfo *field = g_object_info_get_field(info, i);
            list->Set(Nan::New<Number>(i+gcounter), Nan::New<String>(g_base_info_get_name(field)).ToLocalChecked());
            g_base_info_unref(field);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

}
