#include "object.h"
#include "../util.h"
#include "../function.h"
#include "../values.h"

#include <string.h>
#include <node.h>

using namespace v8;

namespace gir {

void empty_func(void) {};

std::vector<ObjectFunctionTemplate> GIRObject::templates;
std::vector<InstanceData> GIRObject::instances;
static Persistent<String> emit_symbol;

GIRObject::GIRObject(GIObjectInfo *info_, int n_params, GParameter *parameters) {
    info = info_;
    GType t = g_registered_type_info_get_g_type(info);
    
    abstract = g_object_info_get_abstract(info);
    if(abstract) {
        obj = NULL;
    }
    else {
        // gobject va_list, to allow construction parameters
        obj = G_OBJECT(g_object_newv(t, n_params, parameters));
    }
}

Handle<Value> GIRObject::New(GObject *obj_, GIObjectInfo *info_) {
    // find the function template
    if(obj_ == NULL || !G_IS_OBJECT(obj_)) {
        return Null();
    }
    
    // very interesting: gtk.Winodw with a child. child.get_parent_window() returns a GIObjetInfo with name GdkWindow (Namespace GDK!)
    // printf("type is %s\n", g_type_name(g_registered_type_info_get_g_type(info_)));
    
    Handle<Value> res = GetInstance(obj_);
    if(res != Null()) {
        return res;
    }
    
    Handle<Value> arg = Boolean::New(false);
    std::vector<ObjectFunctionTemplate>::iterator it;
    
    for(it = templates.begin(); it != templates.end(); ++it) {
    
        if(g_base_info_equal(info_, it->info)) {
            res = it->function->GetFunction()->NewInstance(1, &arg);
            if(!res.IsEmpty()) {
                GIRObject *e = ObjectWrap::Unwrap<GIRObject>(res->ToObject());
                e->info = info_;
                e->obj = obj_;
                e->abstract = false;
                
                return res;
            }
            break;
        }
    }
    return Null();
}

Handle<Value> GIRObject::New(GObject *obj_, GType t) {
    if(obj_ == NULL || !G_IS_OBJECT(obj_)) {
        return Null();
    }
    
    Handle<Value> res = GetInstance(obj_);
    if(res != Null()) {
        return res;
    }
    
    Handle<Value> arg = Boolean::New(false);
    std::vector<ObjectFunctionTemplate>::iterator it;
    
    for(it = templates.begin(); it != templates.end(); ++it) {
        if(t == it->type) {
            res = it->function->GetFunction()->NewInstance(1, &arg);
            if(!res.IsEmpty()) {
                GIRObject *e = ObjectWrap::Unwrap<GIRObject>(res->ToObject());
                e->info = it->info;
                e->obj = obj_;
                e->abstract = false;
                return res;
            }
            return Null();
        }   
    }
}

Handle<Value> GIRObject::New(const Arguments &args) {
    HandleScope scope;
    
    if(args.Length() == 1 && args[0]->IsBoolean() && !args[0]->IsTrue()) {
        GIRObject *obj = new GIRObject();
        obj->Wrap(args.This());
        PushInstance(obj, args.This());
        
        return scope.Close(args.This());
    }

    String::AsciiValue className( args.This()->Get( String::New("__classname__")) );
    std::vector<ObjectFunctionTemplate>::iterator it;
    
    GIObjectInfo *info = NULL;
    for(it = templates.begin(); it != templates.end(); ++it) {
        if(strcmp(it->type_name, *className) == 0) {
            info = it->info;
            break;
        }
    }
    if(info == NULL) {
        return EXCEPTION("no such class. __calssname__ may be incorrect");
    }
    
    int length = 0;
    GParameter *params;
    if(!ToParams(args[0], &params, &length, info)) {
        return BAD_ARGS();
    }
    
    GIRObject *obj = new GIRObject(info, length, params);
    DeleteParams(params, length);
    
    obj->Wrap(args.This());
    PushInstance(obj, args.This());
    
    return scope.Close(args.This());
}

bool GIRObject::ToParams(Handle<Value> val, GParameter** params, int *length, GIObjectInfo *info) {
    *length = 0;
    *params = NULL;
    if(!val->IsObject()) {
        return true;
    }
    Handle<Object> obj = val->ToObject();
    
    Handle<Array> props = obj->GetPropertyNames();
    *length = props->Length();
    *params = new GParameter[*length];
    for(int i=0; i<*length; i++) {
        String::Utf8Value key(props->Get(i)->ToString());
        
        char *name = new char[*length+1];
        strcpy(name, *key);
        
        if(!FindProperty(info, name)) {
            delete[] name;
            DeleteParams(*params, (*length)-1);
            return false;
        }
        
        GValue gvalue = {0,};
        if(!GIRValue::ToGValue(obj->Get(props->Get(i)), G_TYPE_INVALID, &gvalue)) {
            delete[] name;
            DeleteParams(*params, (*length)-1);
            return false;
        }
        
        (*params)[i].name = name;
        (*params)[i].value = gvalue;
    }
    
    return true;
}

void GIRObject::DeleteParams(GParameter* params, int l) {
    for(int i=0; i<l; i++) {
        delete[] params[i].name;
        g_value_unset(&params[i].value);
    }
    delete[] params;
}

void GIRObject::Prepare(Handle<Object> target, GIObjectInfo *info, char *namespace_) {
    HandleScope scope;

    char *name = (char*)g_base_info_get_name(info);
    g_base_info_ref(info);
    
    Local<FunctionTemplate> temp = FunctionTemplate::New(New);
    Persistent<FunctionTemplate> t = Persistent<FunctionTemplate>::New(temp);
    t->SetClassName(String::New(name));
    
    ObjectFunctionTemplate oft;
    oft.type_name = name;
    oft.info = info;
    oft.function = t;
    oft.type = g_registered_type_info_get_g_type(info);
    oft.namespace_ = namespace_;
    
    templates.push_back(oft);
    
    t->InstanceTemplate()->SetInternalFieldCount(1); 
    
    // to identify the object in the constructor
    t->PrototypeTemplate()->Set(String::NewSymbol("__classname__"), String::New(name));
    
    t->Set(String::NewSymbol("__properties__"), PropertyList(info));
    t->Set(String::NewSymbol("__methods__"), MethodList(info));
    t->Set(String::NewSymbol("__interfaces__"), InterfaceList(info));
    t->Set(String::NewSymbol("__fields__"), FieldList(info));
    t->Set(String::NewSymbol("__signals__"), SignalList(info));
    t->Set(String::NewSymbol("__v_funcs__"), VFuncList(info));
    t->Set(String::NewSymbol("__abstract__"), Boolean::New(g_object_info_get_abstract(info)));
    
    int l = g_object_info_get_n_constants(info);
    for(int i=0; i<l; i++) {
        GIConstantInfo *constant = g_object_info_get_constant(info, i);
        t->Set(String::NewSymbol(g_base_info_get_name(constant)), Number::New(i));
        g_base_info_unref(constant);
    }
    
    
    SetPrototypeMethods(t, name);
}

void GIRObject::Initialize(Handle<Object> target, char *namespace_) {
    // this gets called when all classes have been initialized
    std::vector<ObjectFunctionTemplate>::iterator it;
    std::vector<ObjectFunctionTemplate>::iterator temp;
    GIObjectInfo* parent;
    std::vector<const char*> roots;
    Handle<Array> objs = Array::New(templates.size());
    
    int i = 0;
    for(it = templates.begin(); it != templates.end(); ++it) {
        objs->Set(i++, String::New(g_base_info_get_name(it->info)));
        
        parent = g_object_info_get_parent(it->info);
        if(strcmp(it->namespace_, namespace_) != 0 || !parent) {
            continue;
        }

        for(temp = templates.begin(); temp != templates.end(); ++temp) {
            if(g_base_info_equal(temp->info, parent)) {
                it->function->Inherit(temp->function);
                break;
            }
        }
        if(temp == templates.end()) {
            roots.push_back(g_base_info_get_name(it->info));
        }
    }
    for(it = templates.begin(); it != templates.end(); ++it) {
        if(strcmp(it->namespace_, namespace_) == 0) {
            target->Set(String::NewSymbol(g_base_info_get_name(it->info)), it->function->GetFunction());
        }
    }
    
    int rootsLength = roots.size();
    Handle<Array> v8roots = Array::New(rootsLength);
    i = 0;
    for(std::vector<const char*>::iterator it = roots.begin(); it != roots.end(); it++) {
        v8roots->Set(i++, String::New(*it));
    }
    
    target->Set(String::New("__roots__"), v8roots);
    target->Set(String::New("__objects__"), objs);
    
    emit_symbol = NODE_PSYMBOL("emit");
}

void GIRObject::SetPrototypeMethods(Handle<FunctionTemplate> t, char *name) {
    HandleScope scope;
    
    NODE_SET_PROTOTYPE_METHOD(t, "__call__", CallMethod);
    NODE_SET_PROTOTYPE_METHOD(t, "__get_property__", GetProperty);
    NODE_SET_PROTOTYPE_METHOD(t, "__set_property__", SetProperty);
    NODE_SET_PROTOTYPE_METHOD(t, "__get_interface__", GetInterface);
    NODE_SET_PROTOTYPE_METHOD(t, "__get_field__", GetField);
    NODE_SET_PROTOTYPE_METHOD(t, "__watch_signal__", WatchSignal);
    NODE_SET_PROTOTYPE_METHOD(t, "__call_v_func__", CallMethod);
}

Handle<Value> GIRObject::Emit(Handle<Value> argv[], int length) {
    HandleScope scope;
    
    // this will do the magic but dont forget to extend this object in JS from require("events").EventEmitter
    Local<Value> emit_v = handle_->Get(emit_symbol);
    if (!emit_v->IsFunction()) return Null();
    Local<Function> emit = Local<Function>::Cast(emit_v);
    return emit->Call(handle_, length, argv);
}

void GIRObject::PushInstance(GIRObject *obj, Handle<Value> value) {
    Persistent<Object> p_value = Persistent<Object>::New(value->ToObject());
    obj->MakeWeak();
    
    InstanceData data;
    data.obj = obj;
    data.instance = p_value;
    instances.push_back(data);
}

Handle<Value> GIRObject::GetInstance(GObject *obj) {
    std::vector<InstanceData>::iterator it;
    for(it = instances.begin(); it != instances.end(); it++) {
        if(it->obj && it->obj->obj && it->obj->obj == obj) {
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
  gpointer marshal_data) {
    
    MarshalData *data = (MarshalData*)marshal_data;
    
    Handle<Value> args[n_param_values+1];
    args[0] = String::New(data->event_name);
    
    for(int i=0; i<n_param_values; i++) {
        GValue p = param_values[i];
        args[i+1] = GIRValue::FromGValue(&p);
    }
    
    Handle<Value> res = data->that->Emit(args, n_param_values+1);
    if(res != Null()) {
        //GIRValue::ToGValue(res, return_value);
    }
}

void GIRObject::SignalFinalize(gpointer marshal_data, GClosure *c) {
    MarshalData *data = (MarshalData*)marshal_data;
    delete[] data->event_name;
    delete[] data;
}


Handle<Value> GIRObject::CallMethod(const Arguments &args) {
    HandleScope scope;
    
    if(args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS();
    }
    
    String::Utf8Value fname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIFunctionInfo *func = that->FindMethod(that->info, *fname);
    
    if(func) {
        return scope.Close(Func::Call(that->obj, func, args));
    }
    else {
        return EXCEPTION("no such method");
    }
    
    return scope.Close(Undefined());
}

Handle<Value> GIRObject::GetProperty(const Arguments &args) {
    HandleScope scope;
    
    if(args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS();
    }
    
    String::Utf8Value propname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIPropertyInfo *prop = that->FindProperty(that->info, *propname);

    if(!prop) {
        return EXCEPTION("no such property");
    }
    if(!(g_property_info_get_flags(prop) & G_PARAM_READABLE)) {
        return EXCEPTION("property is not readable");
    }
    
    GParamSpec *spec = g_object_class_find_property(G_OBJECT_GET_CLASS(that->obj), *propname);

    GValue gvalue = {0,};
    g_value_init(&gvalue, spec->value_type);
    g_object_get_property(G_OBJECT(that->obj), *propname, &gvalue);
    
    Handle<Value> res = GIRValue::FromGValue(&gvalue);
    g_value_unset(&gvalue);
    
    return scope.Close(res);
}

Handle<Value> GIRObject::SetProperty(const Arguments &args) {
    HandleScope scope;
    
    if(args.Length() < 2 || !args[0]->IsString()) {
        return BAD_ARGS();
    }
    
    String::Utf8Value propname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIPropertyInfo *prop = that->FindProperty(that->info, *propname);
    
    if(!prop) {
        return EXCEPTION("no such property");
    }
    if(!(g_property_info_get_flags(prop) & G_PARAM_WRITABLE)) {
        return EXCEPTION("property is not writable");
    }
    
    GParamSpec *spec = g_object_class_find_property(G_OBJECT_GET_CLASS(that->obj), *propname);
    
    GValue gvalue = {0,};
    if(!GIRValue::ToGValue(args[1], spec->value_type, &gvalue)) {
        return EXCEPTION("Cant convert to JS value to c value");
    }
    g_object_set_property(G_OBJECT(that->obj), *propname, &gvalue);
    
    return scope.Close(Undefined());
}

Handle<Value> GIRObject::GetInterface(const Arguments &args) {
    HandleScope scope;
    
    if(args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS();
    }
    
    String::Utf8Value iname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIInterfaceInfo *interface = that->FindInterface(that->info, *iname);
    
    if(interface) {
        printf("interface %s exsists\n", *iname);
    }
    else {
        printf("interface %s does NOT exsist\n", *iname);
    }
    
    return scope.Close(Undefined());
}

Handle<Value> GIRObject::GetField(const Arguments &args) {
    HandleScope scope;
    
    if(args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS();
    }
    
    String::Utf8Value fname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIFieldInfo *field = that->FindField(that->info, *fname);
    
    if(field) {
        printf("field %s exsists\n", *fname);
    }
    else {
        printf("field %s does NOT exsist\n", *fname);
    }
    
    return scope.Close(Undefined());
}

Handle<Value> GIRObject::WatchSignal(const Arguments &args) {
    HandleScope scope;
    
    if(args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS();
    }
    bool after = true;
    if(args.Length() > 1 && args[1]->IsBoolean()) {
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

Handle<Value> GIRObject::CallVFunc(const Arguments &args) {
    HandleScope scope;
    
    if(args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS();
    }
    
    String::Utf8Value fname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GISignalInfo *vfunc = that->FindSignal(that->info, *fname);
    
    if(vfunc) {
        printf("VFunc %s exsists\n", *fname);
    }
    else {
        printf("VFunc %s does NOT exsist\n", *fname);
    }
    
    return scope.Close(Undefined());
}

GIFunctionInfo *GIRObject::FindMethod(GIObjectInfo *inf, char *name) {
    GIFunctionInfo *func = g_object_info_find_method(inf, name);
    if(!func) {
        GIObjectInfo *parent = g_object_info_get_parent(inf);
        if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(inf) ) != 0) {
            func = FindMethod(parent, name);
        }
        g_base_info_unref(parent);
    }
    return func;
}

GIPropertyInfo *g_object_info_find_property(GIObjectInfo *info, char *name) {
    int l = g_object_info_get_n_properties(info);
    for(int i=0; i<l; i++) {
        GIPropertyInfo *prop = g_object_info_get_property(info, i);
        if(strcmp(g_base_info_get_name(prop), name) == 0) {
            return prop;
        }
        g_base_info_unref(prop);
    }
}

GIPropertyInfo *GIRObject::FindProperty(GIObjectInfo *inf, char *name) {
    GIPropertyInfo *prop = g_object_info_find_property(inf, name);
    if(!prop) {
        GIObjectInfo *parent = g_object_info_get_parent(inf);
        if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(inf) ) != 0) {
            prop = FindProperty(parent, name);
        }
        g_base_info_unref(parent);
    }
    return prop;
}

GIInterfaceInfo *g_object_info_find_interface(GIObjectInfo *info, char *name) {
    int l = g_object_info_get_n_interfaces(info);
    for(int i=0; i<l; i++) {
        GIInterfaceInfo *interface = g_object_info_get_interface(info, i);
        if(strcmp(g_base_info_get_name(interface), name) == 0) {
            return interface;
        }
        g_base_info_unref(interface);
    }
}

GIInterfaceInfo *GIRObject::FindInterface(GIObjectInfo *inf, char *name) {
    GIInterfaceInfo *interface = g_object_info_find_interface(inf, name);
    if(!interface) {
        GIObjectInfo *parent = g_object_info_get_parent(inf);
        if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(inf) ) != 0) {
            interface = FindInterface(parent, name);
        }
        g_base_info_unref(parent);
    }
    return interface;
}

GIFieldInfo *g_object_info_find_field(GIObjectInfo *info, char *name) {
    int l = g_object_info_get_n_fields(info);
    for(int i=0; i<l; i++) {
        GIFieldInfo *field = g_object_info_get_field(info, i);
        if(strcmp(g_base_info_get_name(field), name) == 0) {
            return field;
        }
        g_base_info_unref(field);
    }
}

GIFieldInfo *GIRObject::FindField(GIObjectInfo *inf, char *name) {
    GIFieldInfo *field = g_object_info_find_field(inf, name);
    if(!field) {
        GIObjectInfo *parent = g_object_info_get_parent(inf);
        if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(inf) ) != 0) {
            field = FindField(parent, name);
        }
        g_base_info_unref(parent);
    }
    return field;
}

GISignalInfo *GIRObject::FindSignal(GIObjectInfo *inf, char *name) {
    GISignalInfo *signal = g_object_info_find_signal(inf, name);
    if(!signal) {
        GIObjectInfo *parent = g_object_info_get_parent(inf);
        if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(inf) ) != 0) {
            signal = FindSignal(parent, name);
        }
        g_base_info_unref(parent);
    }
    return signal;
}

GIVFuncInfo *GIRObject::FindVFunc(GIObjectInfo *inf, char *name) {
    GISignalInfo *vfunc = g_object_info_find_vfunc(inf, name);
    if(!vfunc) {
        GIObjectInfo *parent = g_object_info_get_parent(inf);
        if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(inf) ) != 0) {
            vfunc = FindVFunc(parent, name);
        }
        g_base_info_unref(parent);
    }
    return vfunc;
}



Handle<Object> GIRObject::PropertyList(GIObjectInfo *info) {
    Handle<Object> list = Object::New();
    bool first = true;
    int gcounter = 0;
    g_base_info_ref(info);
    
    while(true) {
        if(!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if(!parent) {
                return list;
            }
            if(strcmp(g_base_info_get_name(parent), g_base_info_get_name(info)) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_properties(info);
        for(int i=0; i<l; i++) {
            GIPropertyInfo *prop = g_object_info_get_property(info, i);
            list->Set(Number::New(i+gcounter), String::New(g_base_info_get_name(prop)));
            g_base_info_unref(prop);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::MethodList(GIObjectInfo *info) {
    Handle<Object> list = Object::New();
    bool first = true;
    int gcounter = 0;
    g_base_info_ref(info);
    
    while(true) {
        if(!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if(!parent) {
                return list;
            }
            if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(info) ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_methods(info);
        for(int i=0; i<l; i++) {
            GIFunctionInfo *func = g_object_info_get_method(info, i);
            list->Set(Number::New(i+gcounter), String::New(g_base_info_get_name(func)));
            g_base_info_unref(func);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::InterfaceList(GIObjectInfo *info) {
    Handle<Object> list = Object::New();
    bool first = true;
    int gcounter = 0;
    g_base_info_ref(info);
    
    while(true) {
        if(!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if(!parent) {
                return list;
            }
            if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(info) ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_interfaces(info);
        for(int i=0; i<l; i++) {
            GIInterfaceInfo *interface = g_object_info_get_interface(info, i);
            list->Set(Number::New(i+gcounter), String::New(g_base_info_get_name(interface)));
            g_base_info_unref(interface);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::FieldList(GIObjectInfo *info) {
    Handle<Object> list = Object::New();
    bool first = true;
    int gcounter = 0;
    g_base_info_ref(info);
    
    while(true) {
        if(!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if(!parent) {
                return list;
            }
            if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(info) ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_fields(info);
        for(int i=0; i<l; i++) {
            GIFieldInfo *field = g_object_info_get_field(info, i);
            list->Set(Number::New(i+gcounter), String::New(g_base_info_get_name(field)));
            g_base_info_unref(field);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::SignalList(GIObjectInfo *info) {
    Handle<Object> list = Object::New();
    bool first = true;
    int gcounter = 0;
    g_base_info_ref(info);
    
    while(true) {
        if(!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if(!parent) {
                return list;
            }
            if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(info)/*"InitiallyUnowned"*/ ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_signals(info);
        for(int i=0; i<l; i++) {
            GISignalInfo *signal = g_object_info_get_signal(info, i);
            list->Set(Number::New(i+gcounter), String::New(g_base_info_get_name(signal)));
            g_base_info_unref(signal);
        }
        gcounter += l;
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::VFuncList(GIObjectInfo *info) {
    Handle<Object> list = Object::New();
    bool first = true;
    int gcounter = 0;
    g_base_info_ref(info);
    
    while(true) {
        if(!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if(!parent) {
                return list;
            }
            if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(info) ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_vfuncs(info);
        for(int i=0; i<l; i++) {
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
