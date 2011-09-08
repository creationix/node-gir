#include "function.h"
#include "../namespace_loader.h"
#include "../util.h"
#include "../function.h"

#include <string.h>
#include <node.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

using namespace v8;

namespace gir {


void GIRFunction::Initialize(Handle<Object> target, GIObjectInfo *info) {
    HandleScope scope;
    
    Local<FunctionTemplate> temp = FunctionTemplate::New(Execute);
    temp->GetFunction()->Set(String::New("__fname__"), String::New(g_base_info_get_name(info)));
    
    char *name = ToCamelCase(g_base_info_get_name(info));
    target->Set(String::NewSymbol(name), temp->GetFunction());
    delete[] name;
}


char *GIRFunction::ToCamelCase(const char *str) {
    int l = strlen(str), it = 0;
    char *res = new char[l];
    
    bool up = false;
    for(int i=0; i<l; i++) {
        if(str[i] == '_') {
            up = true;
        }
        else if(!up) {
            res[it++] = str[i];
        }
        else {
            res[it++] = str[i] - 'a' + 'A';
            up = false;
        }
    }
    res[it] = '\0';
    return res;
}

Handle<Value> GIRFunction::Execute(const Arguments &args) {
    HandleScope scope;
    
    String::Utf8Value fname_(args.Callee()->Get(String::New("__fname__")));
    
    GIFunctionInfo *func = g_irepository_find_by_name(NamespaceLoader::repo, NamespaceLoader::active_namespace, *fname_);
    
    if(func) {
        return scope.Close(Func::Call(NULL, func, args));
    }
    else {
        return EXCEPTION("no such function");
    }
    
    return scope.Close(Undefined());
}


}
