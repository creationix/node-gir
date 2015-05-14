#include <iostream>

#include "object.h"
#include "function_type.h"
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
static Handle<String> emit_symbol;
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
        return NanNull();
    }

    Handle<Value> res = GetInstance(obj_);
    if (res != NanNull()) {
        return res;
    }
    Handle<Value> arg = NanNew<Boolean>(false);
    std::vector<ObjectFunctionTemplate>::iterator it;

    GIObjectInfo *object_info = _get_object_info(G_OBJECT_TYPE(obj_), info_);
    if (!object_info) {
        gchar *msg = g_strdup_printf("ObjectInfo not found for '%s'", G_OBJECT_TYPE_NAME(obj_));
        NanThrowTypeError(msg);
    }
    for (it = templates.begin(); it != templates.end(); ++it) {
        if (g_base_info_equal(object_info, it->info)) {
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

    return NanNull();
}

Handle<Value> GIRObject::New(GObject *obj_, GType t) 
{
    if (obj_ == NULL || !G_IS_OBJECT(obj_)) {
        return NanNull();
    }
    
    Handle<Value> res = GetInstance(obj_);
    if (res != NanNull()) {
        return res;
    }
    
    Handle<Value> arg = NanNew<Boolean>(false);
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
            return NanNull();
        }   
    }
    return NanNull();
}

NAN_METHOD(GIRObject::New) 
{
    NanScope();
    
    if (args.Length() == 1 && args[0]->IsBoolean() && !args[0]->IsTrue()) {
        GIRObject *obj = new GIRObject();
        obj->Wrap(args.This());
        PushInstance(obj, args.This());
        NanReturnThis();
    }
 
    NanAsciiString className(args.Callee()->GetName());
    
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
        NanThrowError("no such class. Callee()->GetName() returned wrong classname");
    }
    
    int length = 0;
    GParameter *params = NULL;
    Handle<Value> v = ToParams(args[0], &params, &length, info);
    if (v != NanNull())
        NanReturnValue(v);
  
    GIRObject *obj = new GIRObject(info, length, params);
    DeleteParams(params, length);
    
    obj->Wrap(args.This());
    PushInstance(obj, args.This());
    NanReturnThis();
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
        return NanNull();
    }
    Handle<Object> obj = val->ToObject();
    
    Handle<Array> props = obj->GetPropertyNames();
    *length = props->Length();
    *params = g_new0(GParameter, *length);
    GParamSpec *pspec = NULL;
    GObjectClass *klass = (GObjectClass*) g_type_class_ref(g_type_from_name(g_object_info_get_type_name(info)));
    for (int i=0; i<*length; i++) {
        String::Utf8Value key(props->Get(i)->ToString());

        // nullify name so it can be freed safely
        (*params)[i].name = NULL;
        
        if (!FindProperty(info, *key)) { 
            /* Try to find property spec registered for given class */
            if (klass) {
                pspec = g_object_class_find_property(klass, *key);
            }

            if (!pspec) {
                DeleteParams(*params, (*length)-1);
                gchar *msg = g_strdup_printf("Can not find '%s' property", *key);
                NanThrowTypeError(msg); 
            }
        }
        
        GValue gvalue = {0, {{0}}};
        GType value_type = G_TYPE_INVALID;
        // Determine the best match for property's type
        if (klass) {
            if (!pspec)
                pspec = g_object_class_find_property(klass, *key);

            if (pspec)
                value_type = pspec->value_type;
        } 
        if (!GIRValue::ToGValue(obj->Get(props->Get(i)), value_type, &gvalue)) {
            DeleteParams(*params, (*length)-1);
            gchar *msg = g_strdup_printf("'%s' property value conversion failed", *key);
            NanThrowTypeError(msg); 
        }
        
        (*params)[i].name = g_strdup(*key);
        (*params)[i].value = gvalue;

        pspec = NULL;
    }

    if (klass)
        g_type_class_unref(klass);

    return NanNull();
}

void GIRObject::DeleteParams(GParameter *params, int l) 
{
    if (params == NULL)
        return;

    for (int i=0; i<l; i++) {
        if (params[i].name == NULL) 
            break;
        g_free((gchar *)params[i].name);
        g_value_unset(&params[i].value);
    }
    g_free(params);
}

NAN_PROPERTY_GETTER(PropertyGetHandler) 
{
    NanScope();
    NanAsciiString _name(property);
    v8::Handle<v8::External> info_ptr = v8::Handle<v8::External>::Cast(args.Data());
    GIBaseInfo *base_info  = (GIBaseInfo*) info_ptr->Value();
    if (base_info != NULL) {
        GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
        GParamSpec *pspec = g_object_class_find_property(G_OBJECT_GET_CLASS(that->obj), *_name);
        if (pspec) {
            // Property is not readable
            if (!(pspec->flags & G_PARAM_READABLE)) {
                NanThrowTypeError("property is not readable");
            }
            
            debug_printf("GetHandler (Get property) [%p] '%s.%s' \n", that->obj, G_OBJECT_TYPE_NAME(that->obj), *_name);

            GIPropertyInfo *prop_info = g_object_info_find_property(base_info, *_name);
            GType value_type = G_TYPE_FUNDAMENTAL(pspec->value_type);
            GValue gvalue = {0, {{0}}};
            g_value_init(&gvalue, pspec->value_type);
            g_object_get_property(G_OBJECT(that->obj), *_name, &gvalue);
            Handle<Value> res = GIRValue::FromGValue(&gvalue, prop_info);          
            if (value_type != G_TYPE_OBJECT && value_type != G_TYPE_BOXED) {
                g_value_unset(&gvalue);
            }
            if (prop_info)
                g_base_info_unref(prop_info);

            NanReturnValue(res);
        }
    }

    // Fallback to defaults
    NanReturnValue(args.This()->GetPrototype()->ToObject()->Get(property));
}

NAN_PROPERTY_QUERY(PropertyQueryHandler)
{
    NanScope();
    NanAsciiString _name(property);
    debug_printf("QUERY HANDLER '%s' \n", *_name);
    NanReturnValue(NanNew<Integer>(0));
}

NAN_PROPERTY_SETTER(PropertySetHandler) 
{
    NanScope();
    NanAsciiString _name(property);

    v8::Handle<v8::External> info_ptr = v8::Handle<v8::External>::Cast(args.Data());
    GIBaseInfo *base_info  = (GIBaseInfo*) info_ptr->Value();
    if (base_info != NULL) {
        GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
        GParamSpec *pspec = g_object_class_find_property(G_OBJECT_GET_CLASS(that->obj), *_name);
        if (pspec) {
            // Property is not readable
            if (!(pspec->flags & G_PARAM_WRITABLE)) {
                NanThrowTypeError("property is not writable");
            }
     
            debug_printf("SetHandler (Set property) '%s.%s' \n", G_OBJECT_TYPE_NAME(that->obj), *_name);

            bool value_is_set = false;
            GValue gvalue = {0, {{0}}};
            value_is_set = GIRValue::ToGValue(value, pspec->value_type, &gvalue);
            g_object_set_property(G_OBJECT(that->obj), *_name, &gvalue);
            GType value_type = G_TYPE_FUNDAMENTAL(pspec->value_type);
            if (value_type != G_TYPE_OBJECT && value_type != G_TYPE_BOXED) {
                g_value_unset(&gvalue);
            }          
            NanReturnValue(NanNew<v8::Boolean>(value_is_set));
        }
    }

    // Fallback to defaults
    NanReturnValue(NanNew<v8::Boolean>(args.This()->GetPrototype()->ToObject()->Set(property, value)));
}

void GIRObject::Prepare(Handle<Object> target, GIObjectInfo *info) 
{
    char *name = (char*)g_base_info_get_name(info);
    const char *namespace_ = g_base_info_get_namespace(info);
    g_base_info_ref(info);
    
    Local<FunctionTemplate> t = NanNew<FunctionTemplate>(New);
    t->SetClassName(NanNew<String>(name));
    
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
    v8::Handle<v8::External> info_handle = NanNew<v8::External>((void*)g_base_info_ref(info));
    // Set properties handlers
    instance_t->SetNamedPropertyHandler(PropertyGetHandler, PropertySetHandler, PropertyQueryHandler, 0, 0, info_handle);
        
    t->Set(NanNew<v8::String>("__properties__"), PropertyList(info));
    t->Set(NanNew<v8::String>("__methods__"), MethodList(info));
    t->Set(NanNew<v8::String>("__interfaces__"), InterfaceList(info));
    t->Set(NanNew<v8::String>("__fields__"), FieldList(info));
    t->Set(NanNew<v8::String>("__signals__"), SignalList(info));
    t->Set(NanNew<v8::String>("__v_funcs__"), VFuncList(info));
    t->Set(NanNew<v8::String>("__abstract__"), NanNew<Boolean>(g_object_info_get_abstract(info)));
    
    int l = g_object_info_get_n_constants(info);
    for (int i=0; i<l; i++) {
        GIConstantInfo *constant = g_object_info_get_constant(info, i);
        t->Set(NanNew<String>(g_base_info_get_name(constant)), NanNew<Number>(i));
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
            target->Set(NanNew<String>(g_base_info_get_name(it->info)), it->function->GetFunction());
        }
    }
    
    emit_symbol = NanNew<String>("emit");
}

void GIRObject::SetPrototypeMethods(Handle<FunctionTemplate> t, char *name) 
{
    NODE_SET_PROTOTYPE_METHOD(t, "__get_property__", GetProperty);
    NODE_SET_PROTOTYPE_METHOD(t, "__set_property__", SetProperty);
    NODE_SET_PROTOTYPE_METHOD(t, "__get_interface__", GetInterface);
    NODE_SET_PROTOTYPE_METHOD(t, "__get_field__", GetField);
    NODE_SET_PROTOTYPE_METHOD(t, "__watch_signal__", WatchSignal);
    NODE_SET_PROTOTYPE_METHOD(t, "__call_v_func__", CallMethod);
}

Handle<Value> GIRObject::Emit(Handle<Value> argv[], int length) 
{
    //NanAsciiString cname(handle_->GetConstructorName());
    //String::Utf8Value signal(argv[0]);

    //printf ("Emit, handle is '%s' '%s' [%p], length (%d) \n", *cname, *signal, handle_, length);

    // this will do the magic but dont forget to extend this object in JS from require("events").EventEmitter
    Local<Value> emit_v = handle()->Get(emit_symbol);
    //printf ("EMIT PTR IS [%p] \n", emit_v);
    //v8::String::AsciiValue ename(emit_v->ToString());
    //printf ("Emit, emit is '%s' \n", *ename);
    if (emit_v->IsUndefined() || !emit_v->IsFunction()) {
        return NanNull();
    }

    Local<Function> emit = Local<Function>::Cast(emit_v);
    return emit->Call(handle(), length, argv);
}

void GIRObject::PushInstance(GIRObject *obj, Handle<Value> value) 
{
    Local<Object> p_value = value->ToObject();
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
    return NanNull();
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
    //printf ("SignalCallback : [%p] '%s' \n", data->event_name, data->event_name);
    args[0] = NanNew<String>(data->event_name);
    
    for (guint i=0; i<n_param_values; i++) {
        GValue p = param_values[i];
        args[i+1] = GIRValue::FromGValue(&p, NULL);
    }

    Handle<Value> res = data->that->Emit(args, n_param_values+1);
    if (res != NanNull()) {
        //printf ("Call ToGValue '%s'\n", G_VALUE_TYPE_NAME(return_value));
        if (return_value && G_IS_VALUE(return_value))
            GIRValue::ToGValue(res, G_VALUE_TYPE(return_value), return_value);
    }
}

void GIRObject::SignalFinalize(gpointer marshal_data, GClosure *c) 
{ 
    MarshalData *data = (MarshalData*)marshal_data;
    g_free (data->event_name);
    g_free (data);
}

NAN_METHOD(GIRObject::CallUnknownMethod) 
{
    NanScope();
     
    NanAsciiString fname(args.Callee()->GetName());
    debug_printf("Call method '%s' \n", *fname);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIFunctionInfo *func = that->FindMethod(that->info, *fname);
    debug_printf("Call Method: '%s' [%p] \n", *fname, func);
    
    if (func) {
        debug_printf("\t Call symbol: '%s' \n", g_function_info_get_symbol(func));
        NanReturnValue(Func::Call(that->obj, func, args, TRUE));
    }
    NanThrowError("no such method");
}

NAN_METHOD(GIRObject::CallMethod) 
{
    NanScope();
    
    if (args.Length() < 1 || !args[0]->IsString()) {
        NanThrowError("Invalid argument's number or type");
    }
    
    String::Utf8Value fname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIFunctionInfo *func = that->FindMethod(that->info, *fname);
    
    if (func) {
        NanReturnValue(Func::Call(that->obj, func, args, FALSE));
    }
    else {
        NanThrowError("no such method");
    }
    
    NanReturnUndefined();
}

NAN_METHOD(GIRObject::GetProperty) 
{
    NanScope();
    
    if (args.Length() < 1 || !args[0]->IsString()) {
        NanThrowError("Invalid argument's number or type");
    }
    
    String::Utf8Value propname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIPropertyInfo *prop = that->FindProperty(that->info, *propname);

    if (!prop) {
        NanThrowError("no such property");
    }
    if (!(g_property_info_get_flags(prop) & G_PARAM_READABLE)) {
        NanThrowError("property is not readable");
    }
    
    GParamSpec *spec = g_object_class_find_property(G_OBJECT_GET_CLASS(that->obj), *propname);

    GValue gvalue = {0, {{0}}};
    g_value_init(&gvalue, spec->value_type);
    g_object_get_property(G_OBJECT(that->obj), *propname, &gvalue);
    
    Handle<Value> res = GIRValue::FromGValue(&gvalue, NULL);
    g_value_unset(&gvalue);
    
    NanReturnValue(res);
}

NAN_METHOD(GIRObject::SetProperty) 
{
    NanScope();
    
    if(args.Length() < 2 || !args[0]->IsString()) {
        NanThrowError("Invalid argument's number or type");
    }
    
    String::Utf8Value propname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIPropertyInfo *prop = that->FindProperty(that->info, *propname);
    
    if (!prop) {
        NanThrowError("no such property");
    }
    if (!(g_property_info_get_flags(prop) & G_PARAM_WRITABLE)) {
        NanThrowError("property is not writable");
    }
    
    GParamSpec *spec = g_object_class_find_property(G_OBJECT_GET_CLASS(that->obj), *propname);
    
    GValue gvalue = {0, {{0}}};
    if (!GIRValue::ToGValue(args[1], spec->value_type, &gvalue)) {
        NanThrowError("Cant convert to JS value to c value");
    }
    g_object_set_property(G_OBJECT(that->obj), *propname, &gvalue);
    
    NanReturnUndefined();
}

NAN_METHOD(GIRObject::GetInterface) 
{
    NanScope();
    
    if (args.Length() < 1 || !args[0]->IsString()) {
        NanThrowError("Invalid argument's number or type");
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
    
    NanReturnUndefined();
}

NAN_METHOD(GIRObject::GetField) 
{
    NanScope();
    
    if (args.Length() < 1 || !args[0]->IsString()) {
        NanThrowError("Invalid argument's numer or type");
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
    
    NanReturnUndefined();
}

NAN_METHOD(GIRObject::WatchSignal) 
{
    NanScope();
    
    if (args.Length() < 1 || !args[0]->IsString()) {
        NanThrowError("Invalid argument's number or type");
    }
    bool after = true;
    if (args.Length() > 1 && args[1]->IsBoolean()) {
        after = args[1]->ToBoolean()->IsTrue();
    }
    
    String::Utf8Value sname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GISignalInfo *signal = that->FindSignal(that->info, *sname);
    //printf ("WATCH : OBJ '%s', SIGNAL '%s' \n", G_OBJECT_TYPE_NAME (that->obj), *sname);

    if(signal) {
        MarshalData *data = g_new(MarshalData, 1);
        data->that = that;
        data->event_name = g_strdup(*sname); 
        
        GClosure *closure = g_cclosure_new(G_CALLBACK(empty_func), NULL, NULL);
        g_closure_add_finalize_notifier(closure, data, GIRObject::SignalFinalize);
        g_closure_set_meta_marshal(closure, data, GIRObject::SignalCallback);
        g_signal_connect_closure(that->obj, *sname, closure, after);
    }
    else {
        NanThrowError("no such signal");
    }
    
    NanReturnUndefined();
}

NAN_METHOD(GIRObject::CallVFunc) 
{
    NanScope();
    
    if (args.Length() < 1 || !args[0]->IsString()) {
        NanThrowError("Invalid argument's number or type");
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
    
    NanReturnUndefined();
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

static GISignalInfo*
_object_info_find_interface_signal(GIObjectInfo *inf, char *name)
{
    int ifaces = g_object_info_get_n_interfaces(inf);
    GISignalInfo *signal = NULL;
    for (int i = 0; i < ifaces; i++) {
        GIInterfaceInfo *iface_info = g_object_info_get_interface(inf, i);
        int n_signals = g_interface_info_get_n_signals(iface_info);
        for (int n = 0; n < n_signals; n++) {
            signal = g_interface_info_get_signal(iface_info, n);
            if (g_str_equal(g_base_info_get_name(signal), name)) {
                break;
            }
            g_base_info_unref(signal);
        }
        g_base_info_unref(iface_info);
    }

    return signal;
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
        if (!signal)
            signal = _object_info_find_interface_signal(inf, name);
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
    Handle<Object> list = NanNew<Object>();
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
            list->Set(NanNew<Number>(i+gcounter), NanNew<String>(g_base_info_get_name(prop)));
            g_base_info_unref(prop);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::MethodList(GIObjectInfo *info) 
{
    Handle<Object> list = NanNew<Object>();
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
            list->Set(NanNew<Number>(i+gcounter), NanNew<String>(g_base_info_get_name(func)));
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
            // In such case, do not set prototype method but instead register
            // a function attached to the namespace of the class.
            if (func_flag & GI_FUNCTION_IS_METHOD) {
                NODE_SET_PROTOTYPE_METHOD(t, func_name, CallUnknownMethod);
            } else {
                // Create new function
                Local< Function > callback_func = NanNew<FunctionTemplate>(Func::CallStaticMethod)->GetFunction();
                // Set name
                callback_func->SetName(NanNew<String>(func_name));
                // Create external to hold GIBaseInfo and set it
                v8::Handle<v8::External> info_ptr = NanNew<v8::External>((void*)g_base_info_ref(func));
                callback_func->SetHiddenValue(NanNew<String>("GIInfo"), info_ptr);
                // Set v8 function
                t->Set(NanNew<String>(func_name), callback_func);
            }
            g_base_info_unref(func);
        }
        gcounter += l;
        first = false;
    }
}

Handle<Object> GIRObject::InterfaceList(GIObjectInfo *info) 
{
    Handle<Object> list = NanNew<Object>();
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
            list->Set(NanNew<Number>(i+gcounter), NanNew<String>(g_base_info_get_name(interface)));
            g_base_info_unref(interface);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::FieldList(GIObjectInfo *info) 
{
    Handle<Object> list = NanNew<Object>();
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
            list->Set(NanNew<Number>(i+gcounter), NanNew<String>(g_base_info_get_name(field)));
            g_base_info_unref(field);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::SignalList(GIObjectInfo *info) 
{
    Handle<Object> list = NanNew<Object>();
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
            list->Set(NanNew<Number>(i+gcounter), NanNew<String>(g_base_info_get_name(signal)));
            g_base_info_unref(signal);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::VFuncList(GIObjectInfo *info) 
{
    Handle<Object> list = NanNew<Object>();
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
            list->Set(NanNew<Number>(i+gcounter), NanNew<String>(g_base_info_get_name(vfunc)));
            g_base_info_unref(vfunc);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

}
