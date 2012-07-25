#include <iostream>

#include "object.h"
#include "function.h"
#include "../util.h"
#include "../function.h"
#include "../values.h"
#include "../namespace_loader.h"

#include <string.h>
#include <node.h>

using namespace v8;
using namespace std;

namespace gir {

void empty_func(void) {};

std::vector<ObjectFunctionTemplate> GIRObject::templates;
std::vector<InstanceData> GIRObject::instances;
static Persistent<String> emit_symbol;
GIPropertyInfo *g_object_info_find_property(GIObjectInfo *info, char *name);

GIRObject::GIRObject(GIObjectInfo *info_, int n_params, GParameter *parameters) 
{
    info = info_;
    GType t = g_registered_type_info_get_g_type(info);
    
    abstract = g_object_info_get_abstract(info);
    if (abstract) {
        obj = NULL;
    }
    else {
        // gobject va_list, to allow construction parameters
        obj = G_OBJECT(g_object_newv(t, n_params, parameters)); 
        debug_printf("New GObject [%p] '%s' \n", obj,G_OBJECT_TYPE_NAME(obj));
    }
}

GIObjectInfo *
_get_object_info(GType obj_type, GIObjectInfo *info)
{
    // The main purpose of this function is to get the best matching info.
    // For example, we have class C which extends B and this extends A.
    // Function x() returns C instances, while it's introspected to return A instances.
    // If C info is not found, we try B as it's the first ancestor of C, etc.
    GIBaseInfo *tmp_info = g_irepository_find_by_gtype(NamespaceLoader::repo, obj_type);
    if (tmp_info != NULL && g_base_info_equal(tmp_info, info))
        return g_base_info_ref(tmp_info);
    GType parent_type = g_type_parent(obj_type);
    if (tmp_info == NULL)
        return g_irepository_find_by_gtype(NamespaceLoader::repo, parent_type);
    return tmp_info;
}

Handle<Value> GIRObject::New(GObject *obj_, GIObjectInfo *info_) 
{
    // find the function template
    if (obj_ == NULL || !G_IS_OBJECT(obj_)) {

        return Null();
    }

    Handle<Value> res = GetInstance(obj_);
    if (res != Null()) {
        return res;
    }
    Handle<Value> arg = Boolean::New(false);
    std::vector<ObjectFunctionTemplate>::iterator it;
    
    GIObjectInfo *object_info = _get_object_info(G_OBJECT_TYPE(obj_), info_);
    for (it = templates.begin(); it != templates.end(); ++it) {
       if (object_info != NULL && g_base_info_equal(object_info, it->info)) {
            res = it->function->GetFunction()->NewInstance(1, &arg);
            if (!res.IsEmpty()) {
                GIRObject *e = ObjectWrap::Unwrap<GIRObject>(res->ToObject());
                e->info = object_info;
                e->obj = obj_;
                e->abstract = false;
                g_base_info_unref(object_info);
                return res;
            }
            break;
        }
    }
    if (object_info)
	    g_base_info_unref(object_info);

    return Null();
}

Handle<Value> GIRObject::New(GObject *obj_, GType t) 
{
    if (obj_ == NULL || !G_IS_OBJECT(obj_)) {
        return Null();
    }
    
    Handle<Value> res = GetInstance(obj_);
    if (res != Null()) {
        return res;
    }
    
    Handle<Value> arg = Boolean::New(false);
    std::vector<ObjectFunctionTemplate>::iterator it;
    GIBaseInfo *base_info = g_irepository_find_by_gtype(NamespaceLoader::repo, t);
    if (base_info == NULL) {
        base_info = g_irepository_find_by_gtype(NamespaceLoader::repo, g_type_parent(t));
    }
    /*printf("CREATE NEW OBJECT WITH TYPE '%s' BASE '%s' \n", 
            G_OBJECT_TYPE_NAME(t),
            g_base_info_get_name(base_info)); */
    for (it = templates.begin(); it != templates.end(); ++it) {
        if (t == it->type) {
            res = it->function->GetFunction()->NewInstance(1, &arg);
            if (!res.IsEmpty()) {
                GIRObject *e = ObjectWrap::Unwrap<GIRObject>(res->ToObject());
                e->info = it->info;
                e->obj = obj_;
                e->abstract = false; 
                return res;
            } 
            return Null();
        }   
    }
    return Null();
}

Handle<Value> GIRObject::New(const Arguments &args) 
{
    HandleScope scope;
    
    if (args.Length() == 1 && args[0]->IsBoolean() && !args[0]->IsTrue()) {
        GIRObject *obj = new GIRObject();
        obj->Wrap(args.This());
        PushInstance(obj, args.This());
        
        return scope.Close(args.This());
    }
 
    String::AsciiValue className(args.Callee()->GetName());
    
    debug_printf ("CTR '%s' \n", *className); 
    std::vector<ObjectFunctionTemplate>::iterator it;

    GIObjectInfo *info = NULL;
    for (it = templates.begin(); it != templates.end(); ++it) {
        if (strcmp(it->type_name, *className) == 0) {
            info = it->info;
            break;
        }
    }

    if (info == NULL) {
        return EXCEPTION("no such class. Callee()->GetName() returned wrong classname");
    }
    
    int length = 0;
    GParameter *params = NULL;
    Handle<Value> v = ToParams(args[0], &params, &length, info);
    if (v != Null())
        return v;
  
    GIRObject *obj = new GIRObject(info, length, params);
    DeleteParams(params, length);
    
    obj->Wrap(args.This());
    PushInstance(obj, args.This());
    
    return scope.Close(args.This());
}

GIRObject::~GIRObject() 
{
    // This destructor willbe called only (and only) if object is garabage collected
    // For persistant destructor see Node::AtExit
    // http://prox.moraphi.com/index.php/https/github.com/bnoordhuis/node/commit/1c20cac
}

Handle<Value> GIRObject::ToParams(Handle<Value> val, GParameter** params, int *length, GIObjectInfo *info) 
{ 
    *length = 0;
    *params = NULL;
    if (!val->IsObject()) {
        return Null();
    }
    Handle<Object> obj = val->ToObject();
    
    Handle<Array> props = obj->GetPropertyNames();
    *length = props->Length();
    *params = g_new0(GParameter, *length);
    for (int i=0; i<*length; i++) {
        String::Utf8Value key(props->Get(i)->ToString());

        // nullify name so it can be freed safely
        (*params)[i].name = NULL;
        
        if (!FindProperty(info, *key)) { 
            DeleteParams(*params, (*length)-1);
            gchar *msg = g_strdup_printf("Can not find '%s' property", *key);
            return EXCEPTION(msg); 
        }
        
        GValue gvalue = {0,};
        GType value_type = G_TYPE_INVALID;
        // Determine the best match for property's type
        GObjectClass *klass = (GObjectClass*) g_type_class_ref(g_type_from_name(g_object_info_get_type_name(info)));
        if (klass) {
            GParamSpec *pspec = g_object_class_find_property(klass, *key);
            if (pspec)
                value_type = pspec->value_type;
            g_type_class_unref(klass);
        } 
        if (!GIRValue::ToGValue(obj->Get(props->Get(i)), value_type, &gvalue)) {
            DeleteParams(*params, (*length)-1);
            gchar *msg = g_strdup_printf("'%s' property value conversion failed", *key);
            return EXCEPTION(msg); 
        }
        
        (*params)[i].name = g_strdup(*key);
        (*params)[i].value = gvalue;
    }
    return Null();
}

void GIRObject::DeleteParams(GParameter *params, int l) 
{
    if (params == NULL)
        return;

    for (int i=0; i<l; i++) {
        if (params[i].name == NULL) 
            break;
        delete[] params[i].name;
        g_value_unset(&params[i].value);
    }
    delete[] params;
}

v8::Handle<v8::Value> PropertyGetHandler(v8::Local<v8::String> name, const v8::AccessorInfo &info) 
{
    String::AsciiValue _name(name);
    v8::Handle<v8::External> info_ptr = v8::Handle<v8::External>::Cast(info.Data());
    GIBaseInfo *base_info  = (GIBaseInfo*) info_ptr->Value();
    if (base_info != NULL) {
        GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(info.This()->ToObject());
        GParamSpec *pspec = g_object_class_find_property(G_OBJECT_GET_CLASS(that->obj), *_name);
        if (pspec) {
            // Property is not readable
            if (!(pspec->flags & G_PARAM_READABLE)) {
                return EXCEPTION("property is not readable");
            }
            
            debug_printf("GetHandler (Get property) [%p] '%s.%s' \n", that->obj, G_OBJECT_TYPE_NAME(that->obj), *_name);

            GIPropertyInfo *prop_info = g_object_info_find_property(base_info, *_name);
            GType value_type = G_TYPE_FUNDAMENTAL(pspec->value_type);
            GValue gvalue = {0,};
            g_value_init(&gvalue, pspec->value_type);
            g_object_get_property(G_OBJECT(that->obj), *_name, &gvalue);
            Handle<Value> res = GIRValue::FromGValue(&gvalue, prop_info);          
            if (value_type != G_TYPE_OBJECT && value_type != G_TYPE_BOXED) {
                g_value_unset(&gvalue);
            }
            if (prop_info)
                g_base_info_unref(prop_info);

            return res;
        }
    }

    // Fallback to defaults
    return info.This()->GetPrototype()->ToObject()->Get(name);
}

v8::Handle<v8::Integer> PropertyQueryHandler(v8::Local<v8::String> name, const v8::AccessorInfo &info) 
{
    String::AsciiValue _name(name);
    debug_printf("QUERY HANDLER '%s' \n", *_name);
    return v8::Integer::New(0);
}

v8::Handle<v8::Value> PropertySetHandler(v8::Local<v8::String> name, Local< Value > value, const v8::AccessorInfo &info) 
{
    String::AsciiValue _name(name);

    v8::Handle<v8::External> info_ptr = v8::Handle<v8::External>::Cast(info.Data());
    GIBaseInfo *base_info  = (GIBaseInfo*) info_ptr->Value();
    if (base_info != NULL) {
        GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(info.This()->ToObject());
        GParamSpec *pspec = g_object_class_find_property(G_OBJECT_GET_CLASS(that->obj), *_name);
        if (pspec) {
            // Property is not readable
            if (!(pspec->flags & G_PARAM_WRITABLE)) {
                return EXCEPTION("property is not writable");
            }
     
            debug_printf("SetHandler (Set property) '%s.%s' \n", G_OBJECT_TYPE_NAME(that->obj), *_name);

            bool value_is_set = false;
            GValue gvalue = {0,};
            value_is_set = GIRValue::ToGValue(value, pspec->value_type, &gvalue);
            g_object_set_property(G_OBJECT(that->obj), *_name, &gvalue);
            GType value_type = G_TYPE_FUNDAMENTAL(pspec->value_type);
            if (value_type != G_TYPE_OBJECT && value_type != G_TYPE_BOXED) {
                g_value_unset(&gvalue);
            }          
            return Boolean::New(value_is_set);
        }
    }

    // Fallback to defaults
    return v8::Boolean::New(info.This()->GetPrototype()->ToObject()->Set(name, value));
}

void GIRObject::Prepare(Handle<Object> target, GIObjectInfo *info) 
{
    HandleScope scope;

    char *name = (char*)g_base_info_get_name(info);
    const char *namespace_ = g_base_info_get_namespace(info);
    g_base_info_ref(info);
    
    Local<FunctionTemplate> temp = FunctionTemplate::New(New);
    Persistent<FunctionTemplate> t = Persistent<FunctionTemplate>::New(temp);
    t->SetClassName(String::New(name));
    
    ObjectFunctionTemplate oft;
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
    v8::Handle<v8::External> info_handle = v8::External::New((void*)g_base_info_ref(info));
    // Set properties handlers
    instance_t->SetNamedPropertyHandler(PropertyGetHandler, PropertySetHandler, PropertyQueryHandler, 0, 0, info_handle);
        
    t->Set(String::NewSymbol("__properties__"), PropertyList(info));
    t->Set(String::NewSymbol("__methods__"), MethodList(info));
    t->Set(String::NewSymbol("__interfaces__"), InterfaceList(info));
    t->Set(String::NewSymbol("__fields__"), FieldList(info));
    t->Set(String::NewSymbol("__signals__"), SignalList(info));
    t->Set(String::NewSymbol("__v_funcs__"), VFuncList(info));
    t->Set(String::NewSymbol("__abstract__"), Boolean::New(g_object_info_get_abstract(info)));
    
    int l = g_object_info_get_n_constants(info);
    for (int i=0; i<l; i++) {
        GIConstantInfo *constant = g_object_info_get_constant(info, i);
        t->Set(String::NewSymbol(g_base_info_get_name(constant)), Number::New(i));
        g_base_info_unref(constant);
    }
    
    RegisterMethods(target, info, namespace_, t);
    SetPrototypeMethods(t, name);
}

void GIRObject::Initialize(Handle<Object> target, char *namespace_) 
{
    // this gets called when all classes have been initialized
    std::vector<ObjectFunctionTemplate>::iterator it;
    std::vector<ObjectFunctionTemplate>::iterator temp;
    GIObjectInfo* parent;
    
    for (it = templates.begin(); it != templates.end(); ++it) {
        parent = g_object_info_get_parent(it->info);
        if (strcmp(it->namespace_, namespace_) != 0 || !parent) {
            continue;
        }

        for (temp = templates.begin(); temp != templates.end(); ++temp) {
            if (g_base_info_equal(temp->info, parent)) {
                it->function->Inherit(temp->function);
            }
        }
    }
    for (it = templates.begin(); it != templates.end(); ++it) {
        if (strcmp(it->namespace_, namespace_) == 0) {
            target->Set(String::NewSymbol(g_base_info_get_name(it->info)), it->function->GetFunction());
        }
    }
    
    emit_symbol = NODE_PSYMBOL("emit");
}

void GIRObject::SetPrototypeMethods(Handle<FunctionTemplate> t, char *name) 
{
    HandleScope scope;
 
    NODE_SET_PROTOTYPE_METHOD(t, "__get_property__", GetProperty);
    NODE_SET_PROTOTYPE_METHOD(t, "__set_property__", SetProperty);
    NODE_SET_PROTOTYPE_METHOD(t, "__get_interface__", GetInterface);
    NODE_SET_PROTOTYPE_METHOD(t, "__get_field__", GetField);
    NODE_SET_PROTOTYPE_METHOD(t, "__watch_signal__", WatchSignal);
    NODE_SET_PROTOTYPE_METHOD(t, "__call_v_func__", CallMethod);
}

Handle<Value> GIRObject::Emit(Handle<Value> argv[], int length) 
{
    HandleScope scope;
    
    // this will do the magic but dont forget to extend this object in JS from require("events").EventEmitter
    Local<Value> emit_v = handle_->Get(emit_symbol);
    if (!emit_v->IsFunction()) return Null();
    Local<Function> emit = Local<Function>::Cast(emit_v);
    return emit->Call(handle_, length, argv);
}

void GIRObject::PushInstance(GIRObject *obj, Handle<Value> value) 
{
    Persistent<Object> p_value = Persistent<Object>::New(value->ToObject());
    obj->MakeWeak();
    
    InstanceData data;
    data.obj = obj;
    data.instance = p_value;
    instances.push_back(data);
}

Handle<Value> GIRObject::GetInstance(GObject *obj) 
{
    std::vector<InstanceData>::iterator it;
    for (it = instances.begin(); it != instances.end(); it++) {
        if (it->obj && it->obj->obj && it->obj->obj == obj) {
            return it->instance;
        }
    }
    return Null();
}

void GIRObject::SignalCallback(GClosure *closure,
  GValue *return_value,
  guint n_param_values,
  const GValue *param_values,
  gpointer invocation_hint,
  gpointer marshal_data) 
{    
    MarshalData *data = (MarshalData*)marshal_data;
    
    Handle<Value> args[n_param_values+1];
    args[0] = String::New(data->event_name);
    
    for (guint i=0; i<n_param_values; i++) {
        GValue p = param_values[i];
        args[i+1] = GIRValue::FromGValue(&p, NULL);
    }
    
    Handle<Value> res = data->that->Emit(args, n_param_values+1);
    if (res != Null()) {
        //GIRValue::ToGValue(res, return_value);
    }
}

void GIRObject::SignalFinalize(gpointer marshal_data, GClosure *c) 
{
    MarshalData *data = (MarshalData*)marshal_data;
    delete[] data->event_name;
    delete[] data;
}

Handle<Value> GIRObject::CallUnknownMethod(const Arguments &args) 
{
    HandleScope scope;
     
    v8::String::AsciiValue fname(args.Callee()->GetName());
    debug_printf("Call method '%s' \n", *fname);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIFunctionInfo *func = that->FindMethod(that->info, *fname);
    debug_printf("Call Method: '%s' [%p] \n", *fname, func);
    
    if (func) {
        debug_printf("\t Call symbol: '%s' \n", g_function_info_get_symbol(func));
        return scope.Close(Func::Call(that->obj, func, args, TRUE));
    }
    return EXCEPTION("no such method");
}

Handle<Value> GIRObject::CallStaticMethod(const Arguments &args) 
{
    HandleScope scope;
        
    v8::String::AsciiValue fname(args.Callee()->GetName());
    v8::Handle<v8::External> info_ptr = 
        v8::Handle<v8::External>::Cast(args.Callee()->GetHiddenValue(String::New("GIInfo")));
    GIBaseInfo *func  = (GIBaseInfo*) info_ptr->Value();
    debug_printf("Call static method '%s'.'%s' ('%s') \n",
            g_base_info_get_namespace(func),
            g_base_info_get_name(func), 
            g_function_info_get_symbol(func));
    
    if (func) {
        return scope.Close(Func::Call(NULL, func, args, TRUE));
    }
    else {
        return EXCEPTION("no such method");
    }
    
    return scope.Close(Undefined());
}

Handle<Value> GIRObject::CallMethod(const Arguments &args) 
{
    HandleScope scope;
    
    if (args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS("Invalid argument's number or type");
    }
    
    String::Utf8Value fname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIFunctionInfo *func = that->FindMethod(that->info, *fname);
    
    if (func) {
        return scope.Close(Func::Call(that->obj, func, args, FALSE));
    }
    else {
        return EXCEPTION("no such method");
    }
    
    return scope.Close(Undefined());
}

Handle<Value> GIRObject::GetProperty(const Arguments &args) 
{
    HandleScope scope;
    
    if (args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS("Invalid argument's number or type");
    }
    
    String::Utf8Value propname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIPropertyInfo *prop = that->FindProperty(that->info, *propname);

    if (!prop) {
        return EXCEPTION("no such property");
    }
    if (!(g_property_info_get_flags(prop) & G_PARAM_READABLE)) {
        return EXCEPTION("property is not readable");
    }
    
    GParamSpec *spec = g_object_class_find_property(G_OBJECT_GET_CLASS(that->obj), *propname);

    GValue gvalue = {0,};
    g_value_init(&gvalue, spec->value_type);
    g_object_get_property(G_OBJECT(that->obj), *propname, &gvalue);
    
    Handle<Value> res = GIRValue::FromGValue(&gvalue, NULL);
    g_value_unset(&gvalue);
    
    return scope.Close(res);
}

Handle<Value> GIRObject::SetProperty(const Arguments &args) 
{
    HandleScope scope;
    
    if(args.Length() < 2 || !args[0]->IsString()) {
        return BAD_ARGS("Invalid argument's number or type");
    }
    
    String::Utf8Value propname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIPropertyInfo *prop = that->FindProperty(that->info, *propname);
    
    if (!prop) {
        return EXCEPTION("no such property");
    }
    if (!(g_property_info_get_flags(prop) & G_PARAM_WRITABLE)) {
        return EXCEPTION("property is not writable");
    }
    
    GParamSpec *spec = g_object_class_find_property(G_OBJECT_GET_CLASS(that->obj), *propname);
    
    GValue gvalue = {0,};
    if (!GIRValue::ToGValue(args[1], spec->value_type, &gvalue)) {
        return EXCEPTION("Cant convert to JS value to c value");
    }
    g_object_set_property(G_OBJECT(that->obj), *propname, &gvalue);
    
    return scope.Close(Undefined());
}

Handle<Value> GIRObject::GetInterface(const Arguments &args) 
{
    HandleScope scope;
    
    if (args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS("Invalid argument's number or type");
    }
    
    String::Utf8Value iname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIInterfaceInfo *interface = that->FindInterface(that->info, *iname);
    
    if (interface) {
        debug_printf("interface %s exists\n", *iname);
    }
    else {
        debug_printf("interface %s does NOT exist\n", *iname);
    }
    
    return scope.Close(Undefined());
}

Handle<Value> GIRObject::GetField(const Arguments &args) 
{
    HandleScope scope;
    
    if (args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS("Invalid argument's numer or type");
    }
    
    String::Utf8Value fname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIFieldInfo *field = that->FindField(that->info, *fname);
    
    if (field) {
        debug_printf("field %s exists\n", *fname);
    }
    else {
        debug_printf("field %s does NOT exist\n", *fname);
    }
    
    return scope.Close(Undefined());
}

Handle<Value> GIRObject::WatchSignal(const Arguments &args) 
{
    HandleScope scope;
    
    if (args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS("Invalid argument's number or type");
    }
    bool after = true;
    if (args.Length() > 1 && args[1]->IsBoolean()) {
        after = args[1]->ToBoolean()->IsTrue();
    }
    
    String::Utf8Value sname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GISignalInfo *signal = that->FindSignal(that->info, *sname);
    
    if(signal) {
        MarshalData *data = new MarshalData();
        data->that = that;
        data->event_name = new char[strlen(*sname)];
        strcpy(data->event_name, *sname);
        
        GClosure *closure = g_cclosure_new(G_CALLBACK(empty_func), NULL, NULL);
        g_closure_add_finalize_notifier(closure, data, GIRObject::SignalFinalize);
        g_closure_set_meta_marshal(closure, data, GIRObject::SignalCallback);
        g_signal_connect_closure(that->obj, *sname, closure, after);
    }
    else {
        EXCEPTION("no such signal");
    }
    
    return scope.Close(Undefined());
}

Handle<Value> GIRObject::CallVFunc(const Arguments &args) 
{
    HandleScope scope;
    
    if (args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS("Invalid argument's number or type");
    }
    
    String::Utf8Value fname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GISignalInfo *vfunc = that->FindSignal(that->info, *fname);
    
    if (vfunc) {
        debug_printf("VFunc %s exists\n", *fname);
    }
    else {
        debug_printf("VFunc %s does NOT exist\n", *fname);
    }
    
    return scope.Close(Undefined());
}

GIFunctionInfo *GIRObject::FindMethod(GIObjectInfo *info, char *name) 
{
    GIFunctionInfo *func = g_object_info_find_method(info, name);

    // Find interface method
    if (!func) {
        int ifaces = g_object_info_get_n_interfaces(info);
        for (int i = 0; i < ifaces; i++) {
            GIInterfaceInfo *iface_info = g_object_info_get_interface(info, i);
            func = g_interface_info_find_method(iface_info, name);
            if (func) {
                g_base_info_unref(iface_info);
                return func;
            }
            g_base_info_unref(iface_info);
        }
    }

    if (!func) {
        GIObjectInfo *parent = g_object_info_get_parent(info);
        func = FindMethod(parent, name);
        g_base_info_unref(parent);
    }
    return func;
}

GIPropertyInfo *g_object_info_find_property(GIObjectInfo *info, char *name) 
{
    int l = g_object_info_get_n_properties(info);
    for (int i=0; i<l; i++) {
        GIPropertyInfo *prop = g_object_info_get_property(info, i);
        if (strcmp(g_base_info_get_name(prop), name) == 0) {
            return prop;
        }
        g_base_info_unref(prop);
    }
    return NULL;
}

GIPropertyInfo *GIRObject::FindProperty(GIObjectInfo *inf, char *name) 
{
    GIPropertyInfo *prop = g_object_info_find_property(inf, name);
    if (!prop) {
        GIObjectInfo *parent = g_object_info_get_parent(inf);
        if (!parent) 
            return NULL;
        if (parent != inf) {
        //if (strcmp( g_base_info_get_name(parent), g_base_info_get_name(inf) ) != 0) {
            prop = FindProperty(parent, name);
        }
        g_base_info_unref(parent);
    }
    return prop;
}

GIInterfaceInfo *g_object_info_find_interface(GIObjectInfo *info, char *name) 
{
    int l = g_object_info_get_n_interfaces(info);
    for (int i=0; i<l; i++) {
        GIInterfaceInfo *interface = g_object_info_get_interface(info, i);
        if (strcmp(g_base_info_get_name(interface), name) == 0) {
            return interface;
        }
        g_base_info_unref(interface);
    }
    return NULL;
}

GIInterfaceInfo *GIRObject::FindInterface(GIObjectInfo *inf, char *name) 
{
    GIInterfaceInfo *interface = g_object_info_find_interface(inf, name);
    if (!interface) {
        GIObjectInfo *parent = g_object_info_get_parent(inf);
        if (strcmp( g_base_info_get_name(parent), g_base_info_get_name(inf) ) != 0) {
            interface = FindInterface(parent, name);
        }
        g_base_info_unref(parent);
    }
    return interface;
}

GIFieldInfo *g_object_info_find_field(GIObjectInfo *info, char *name) 
{
    int l = g_object_info_get_n_fields(info);
    for (int i=0; i<l; i++) {
        GIFieldInfo *field = g_object_info_get_field(info, i);
        if (strcmp(g_base_info_get_name(field), name) == 0) {
            return field;
        }
        g_base_info_unref(field);
    }
    return NULL;
}

GIFieldInfo *GIRObject::FindField(GIObjectInfo *inf, char *name) 
{
    GIFieldInfo *field = g_object_info_find_field(inf, name);
    if (!field) {
        GIObjectInfo *parent = g_object_info_get_parent(inf);
        if (strcmp( g_base_info_get_name(parent), g_base_info_get_name(inf) ) != 0) {
            field = FindField(parent, name);
        }
        g_base_info_unref(parent);
    }
    return field;
}

GISignalInfo *GIRObject::FindSignal(GIObjectInfo *inf, char *name) 
{
    GISignalInfo *signal = g_object_info_find_signal(inf, name);
    if (!signal) {
        GIObjectInfo *parent = g_object_info_get_parent(inf);
        if (parent) {
            if (strcmp( g_base_info_get_name(parent), g_base_info_get_name(inf) ) != 0) {
                signal = FindSignal(parent, name);
            }
            g_base_info_unref(parent);
        }
    }
    return signal;
}

GIVFuncInfo *GIRObject::FindVFunc(GIObjectInfo *inf, char *name) 
{
    GISignalInfo *vfunc = g_object_info_find_vfunc(inf, name);
    if (!vfunc) {
        GIObjectInfo *parent = g_object_info_get_parent(inf);
        if (strcmp( g_base_info_get_name(parent), g_base_info_get_name(inf) ) != 0) {
            vfunc = FindVFunc(parent, name);
        }
        g_base_info_unref(parent);
    }
    return vfunc;
}

Handle<Object> GIRObject::PropertyList(GIObjectInfo *info) 
{
    Handle<Object> list = Object::New();
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
            list->Set(Number::New(i+gcounter), String::New(g_base_info_get_name(prop)));
            g_base_info_unref(prop);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::MethodList(GIObjectInfo *info) 
{
    Handle<Object> list = Object::New();
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
            list->Set(Number::New(i+gcounter), String::New(g_base_info_get_name(func)));
            g_base_info_unref(func);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

void GIRObject::RegisterMethods(Handle<Object> target, GIObjectInfo *info, const char *namespace_, Handle<FunctionTemplate> t) 
{
    bool is_object_info = GI_IS_OBJECT_INFO(info);
    // Register interface methods
    if (is_object_info) {
        int ifaces = g_object_info_get_n_interfaces(info);
        for (int i = 0; i < ifaces; i++) {
            GIInterfaceInfo *iface_info = g_object_info_get_interface(info, i);
            // Register prerequisities
            int n_pre = g_interface_info_get_n_prerequisites(iface_info);
            for (int j = 0; j < n_pre; j++) {
                GIBaseInfo *pre_info = g_interface_info_get_prerequisite(iface_info, j);
                GIRObject::RegisterMethods(target, pre_info, namespace_, t);
                g_base_info_unref(pre_info);
            }
            GIRObject::RegisterMethods(target, iface_info, namespace_, t);
            g_base_info_unref(iface_info);
        }
    }

    bool first = true;
    int gcounter = 0;
    g_base_info_ref(info);
    
    while (true) {
        if (!first) {
            GIObjectInfo *parent = NULL;
            if (GI_IS_OBJECT_INFO(info)) 
                parent = g_object_info_get_parent(info);
            if (!parent) {
                g_base_info_unref(info);
                return;
            } 
            if (g_base_info_equal(parent, info)) {
                g_base_info_unref(info);
                return;
            }
            g_base_info_unref(info);
            info = parent;
        }
       
        int l = 0;
        if (is_object_info) {
            l = g_object_info_get_n_methods(info);
        } else {
            l = g_interface_info_get_n_methods(info);
        }

        for (int i=0; i<l; i++) {
            GIFunctionInfo *func = NULL;
            if (is_object_info) {
                func = g_object_info_get_method(info, i);
            } else {
                func = g_interface_info_get_method(info, i);
            }
            const char *func_name = g_base_info_get_name(func);
            GIFunctionInfoFlags func_flag = g_function_info_get_flags(func);
            // Determine if method is static one.
            // If given function is neither method nor constructor, it's most likely static method.
            // In such case, do not set prototype method.
            if (func_flag & GI_FUNCTION_IS_METHOD) {
                NODE_SET_PROTOTYPE_METHOD(t, func_name, CallUnknownMethod);
            } else if (!(func_flag & GI_FUNCTION_IS_CONSTRUCTOR)) {
                // Create new function
                Local< Function > callback_func = FunctionTemplate::New(CallStaticMethod)->GetFunction();
                // Set name
                callback_func->SetName(String::New(func_name));
                // Create external to hold GIBaseInfo and set it
                v8::Handle<v8::External> info_ptr = v8::External::New((void*)g_base_info_ref(func));
                callback_func->SetHiddenValue(String::New("GIInfo"), info_ptr);
                // Set v8 function
                t->Set(String::NewSymbol(func_name), callback_func);
            }
            g_base_info_unref(func);
        }
        gcounter += l;
        first = false;
    }
}

Handle<Object> GIRObject::InterfaceList(GIObjectInfo *info) 
{
    Handle<Object> list = Object::New();
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
        
        int l = g_object_info_get_n_interfaces(info);
        for (int i=0; i<l; i++) {
            GIInterfaceInfo *interface = g_object_info_get_interface(info, i);
            list->Set(Number::New(i+gcounter), String::New(g_base_info_get_name(interface)));
            g_base_info_unref(interface);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::FieldList(GIObjectInfo *info) 
{
    Handle<Object> list = Object::New();
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
            list->Set(Number::New(i+gcounter), String::New(g_base_info_get_name(field)));
            g_base_info_unref(field);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::SignalList(GIObjectInfo *info) 
{
    Handle<Object> list = Object::New();
    bool first = true;
    int gcounter = 0;
    g_base_info_ref(info);
    
    while (true) {
        if (!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if (!parent) {
                return list;
            }
            if (strcmp( g_base_info_get_name(parent), g_base_info_get_name(info)/*"InitiallyUnowned"*/ ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_signals(info);
        for (int i=0; i<l; i++) {
            GISignalInfo *signal = g_object_info_get_signal(info, i);
            list->Set(Number::New(i+gcounter), String::New(g_base_info_get_name(signal)));
            g_base_info_unref(signal);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::VFuncList(GIObjectInfo *info) 
{
    Handle<Object> list = Object::New();
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
        
        int l = g_object_info_get_n_vfuncs(info);
        for (int i=0; i<l; i++) {
            GIVFuncInfo *vfunc = g_object_info_get_vfunc(info, i);
            list->Set(Number::New(i+gcounter), String::New(g_base_info_get_name(vfunc)));
            g_base_info_unref(vfunc);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

}
