#include "object.h"
#include "util.h"

#include <string.h>
#include <node.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

using namespace v8;

namespace gir {

std::map<char *, GIObjectInfo*> GIRObject::objects;
std::map<GIObjectInfo*, Persistent<FunctionTemplate> > GIRObject::templates;
static Persistent<String> emit_symbol;

GIRObject::GIRObject(GIObjectInfo *info_) {
    info = info_;
    GType t = g_registered_type_info_get_g_type(info);
    
    // TODO: check if abstract class; if so, dont call g_object_new
    // FIXME: some types dont work (GTK_TYPE_WINDOW, GTK_TYPE_MENU)
    abstract = g_object_info_get_abstract(info);
    if(abstract) {
        obj = NULL;
    }
    else {
        obj = G_OBJECT(g_object_new(t, NULL));
    }
}

Handle<Value> GIRObject::New(const Arguments &args) {

    String::AsciiValue className( args.This()->Get( String::New("__classname__")) );
    std::map<char *, GIObjectInfo*>::iterator it;
    
    GIObjectInfo *info = NULL;
    for(it = objects.begin(); it != objects.end(); ++it) {
        if(strcmp(it->first, *className) == 0) {
            info = it->second;
        }
    }
    if(info == NULL) {
        return Null();
    }
    
    GIRObject *obj = new GIRObject(info);
    obj->Wrap(args.This());
    return args.This();
}

void GIRObject::Initialize(Handle<Object> target, GIObjectInfo *info) {
    HandleScope scope;

    const char *name_ = g_base_info_get_name(info);
    char *name = new char[strlen(name_)];
    strcpy(name, name_);
    g_base_info_ref(info);
    objects.insert(std::make_pair(name, info));
    
    Local<FunctionTemplate> temp = FunctionTemplate::New(New);
    Persistent<FunctionTemplate> t = Persistent<FunctionTemplate>::New(temp);
    t->SetClassName(String::New(name));
    templates.insert(std::make_pair(info, t));
    
    // dont do the inherit thing here. wait for all classes to be created and inherit in namespace loader
    //constructor_template->Inherit(Base::constructor_template);
    
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
    
    emit_symbol = NODE_PSYMBOL("emit");
    
    target->Set(String::NewSymbol(name), t->GetFunction());
}

void GIRObject::Inherit(void) {
    // this is gets called when all classes have been initialized
    std::map<GIObjectInfo*, Persistent<FunctionTemplate> >::iterator it;
    std::map<GIObjectInfo*, Persistent<FunctionTemplate> >::iterator temp;
    GIObjectInfo* parent;
    
    for(it = templates.begin(); it != templates.end(); ++it) {
        parent = g_object_info_get_parent(it->first);

        for(temp = templates.begin(); temp != templates.end(); ++temp) {
            if(g_base_info_equal(temp->first, parent)) {
                //printf("inherit %s from %s\n", g_base_info_get_name(it->first), g_base_info_get_name(temp->first) );
                it->second->Inherit(temp->second);
            }
        }
    }
}

void GIRObject::SetPrototypeMethods(Handle<FunctionTemplate> t, char *name) {
    HandleScope scope;
    NODE_SET_PROTOTYPE_METHOD(t, "__call__", CallMethod);
    NODE_SET_PROTOTYPE_METHOD(t, "__get_property__", GetProperty);
    NODE_SET_PROTOTYPE_METHOD(t, "__set_property__", SetProperty);
    NODE_SET_PROTOTYPE_METHOD(t, "__get_interface__", GetInterface);
    NODE_SET_PROTOTYPE_METHOD(t, "__get_field__", GetField);
    NODE_SET_PROTOTYPE_METHOD(t, "__watch_signal__", WatchSignal);
    NODE_SET_PROTOTYPE_METHOD(t, "__call_v_func__", WatchSignal);
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
        printf("method %s exsists\n", *fname);
    }
    else {
        printf("method %s does NOT exsist\n", *fname);
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
    
    if(prop) {
        printf("property %s exsists\n", *propname);
    }
    else {
        printf("property %s does NOT exsist\n", *propname);
    }
    
    return scope.Close(Undefined());
}

Handle<Value> GIRObject::SetProperty(const Arguments &args) {
    HandleScope scope;
    
    if(args.Length() < 1 || !args[0]->IsString()) {
        return BAD_ARGS();
    }
    
    String::Utf8Value propname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GIPropertyInfo *prop = that->FindProperty(that->info, *propname);
    
    if(prop) {
        printf("property %s exsists\n", *propname);
    }
    else {
        printf("property %s does NOT exsist\n", *propname);
    }
    
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
    
    String::Utf8Value sname(args[0]);
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    GISignalInfo *signal = that->FindSignal(that->info, *sname);
    
    if(signal) {
        printf("signal %s exsists\n", *sname);
    }
    else {
        printf("signal %s does NOT exsist\n", *sname);
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
    g_base_info_ref(info);
    
    while(true) {
        if(!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(info) ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_properties(info);
        for(int i=0; i<l; i++) {
            GIPropertyInfo *prop = g_object_info_get_property(info, i);
            list->Set(Number::New(i), String::New(g_base_info_get_name(prop)));
            g_base_info_unref(prop);
        }
        
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::MethodList(GIObjectInfo *info) {
    Handle<Object> list = Object::New();
    bool first = true;
    g_base_info_ref(info);
    
    while(true) {
        if(!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(info) ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_methods(info);
        for(int i=0; i<l; i++) {
            GIFunctionInfo *func = g_object_info_get_method(info, i);
            list->Set(Number::New(i), String::New(g_base_info_get_name(func)));
            g_base_info_unref(func);
        }
        
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::InterfaceList(GIObjectInfo *info) {
    Handle<Object> list = Object::New();
    bool first = true;
    g_base_info_ref(info);
    
    while(true) {
        if(!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(info) ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_interfaces(info);
        for(int i=0; i<l; i++) {
            GIInterfaceInfo *interface = g_object_info_get_interface(info, i);
            list->Set(Number::New(i), String::New(g_base_info_get_name(interface)));
            g_base_info_unref(interface);
        }
        
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::FieldList(GIObjectInfo *info) {
    Handle<Object> list = Object::New();
    bool first = true;
    g_base_info_ref(info);
    
    while(true) {
        if(!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(info) ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_fields(info);
        for(int i=0; i<l; i++) {
            GIFieldInfo *field = g_object_info_get_field(info, i);
            list->Set(Number::New(i), String::New(g_base_info_get_name(field)));
            g_base_info_unref(field);
        }
        
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::SignalList(GIObjectInfo *info) {
    Handle<Object> list = Object::New();
    bool first = true;
    g_base_info_ref(info);
    
    while(true) {
        if(!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(info) ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_signals(info);
        for(int i=0; i<l; i++) {
            GISignalInfo *signal = g_object_info_get_signal(info, i);
            list->Set(Number::New(i), String::New(g_base_info_get_name(signal)));
            g_base_info_unref(signal);
        }
        
        first = false;
    }
    
    return list;
}

Handle<Object> GIRObject::VFuncList(GIObjectInfo *info) {
    Handle<Object> list = Object::New();
    bool first = true;
    g_base_info_ref(info);
    
    while(true) {
        if(!first) {
            GIObjectInfo *parent = g_object_info_get_parent(info);
            if(strcmp( g_base_info_get_name(parent), g_base_info_get_name(info) ) == 0) {
                return list;
            }
            g_base_info_unref(info);
            info = parent;
        }
        
        int l = g_object_info_get_n_vfuncs(info);
        for(int i=0; i<l; i++) {
            GIVFuncInfo *vfunc = g_object_info_get_vfunc(info, i);
            list->Set(Number::New(i), String::New(g_base_info_get_name(vfunc)));
            g_base_info_unref(vfunc);
        }
        
        first = false;
    }
    
    return list;
}


}
