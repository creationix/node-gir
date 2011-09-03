#include "object.h"
#include "util.h"

#include <string.h>
#include <node.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

using namespace v8;

namespace gir {

std::map<char *, GIObjectInfo*> GIRObject::objects;
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
    
    Local<FunctionTemplate> t = FunctionTemplate::New(New);
    t->SetClassName(String::New(name));
    // TODO: inherit
    
    t->InstanceTemplate()->SetInternalFieldCount(1); 
    
    // to identify the object in the constructor
    t->PrototypeTemplate()->Set(String::NewSymbol("__classname__"), String::New(name));
    
    t->Set(String::NewSymbol("__properties__"), PropertyList(info));
    t->Set(String::NewSymbol("__methods__"), MethodList(info));
    t->Set(String::NewSymbol("__interfaces__"), InterfaceList(info));
    t->Set(String::NewSymbol("__fields__"), FieldList(info));
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

void GIRObject::SetPrototypeMethods(Handle<FunctionTemplate> t, char *name) {
    HandleScope scope;
    NODE_SET_PROTOTYPE_METHOD(t, "__call__", CallMethod);
    NODE_SET_PROTOTYPE_METHOD(t, "__get_property__", GetProperty);
    NODE_SET_PROTOTYPE_METHOD(t, "__set_property__", SetProperty);
    NODE_SET_PROTOTYPE_METHOD(t, "__get_interface__", GetInterface);
    NODE_SET_PROTOTYPE_METHOD(t, "__get_field__", GetField);
    NODE_SET_PROTOTYPE_METHOD(t, "ref", Ref);
    NODE_SET_PROTOTYPE_METHOD(t, "unref", Unref);
}


Handle<Value> GIRObject::Unref(const Arguments &args) {
    HandleScope scope;
    
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    //FIXME: process hangs up
    g_object_info_get_unref_function_pointer(that->info)(that->obj);
    
    return scope.Close(Undefined());
}

Handle<Value> GIRObject::Ref(const Arguments &args) {
    HandleScope scope;
    
    GIRObject *that = node::ObjectWrap::Unwrap<GIRObject>(args.This()->ToObject());
    //FIXME: process hangs up
    g_object_info_get_ref_function_pointer(that->info)(that->obj);
    
    return scope.Close(Undefined());
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


}
