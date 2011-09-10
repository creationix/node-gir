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

/*
FIXME: this is reeeealy ugly. we should find onother way to find the namespace and function name.

*/


void GIRFunction::Initialize(Handle<Object> target, GIObjectInfo *info, char *namespace_) {
    HandleScope scope;
    
    const char *fname = g_base_info_get_name(info);
    char *name = new char[strlen(namespace_) + strlen(fname) + 2];
    strcpy(name, namespace_);
    name[strlen(namespace_)] = ':';
    strcpy(name+strlen(namespace_)+1, fname);
    
    Local<FunctionTemplate> temp = FunctionTemplate::New(Execute);
    temp->GetFunction()->Set(String::New("__fname__"), String::New(name));
    
    char *jsname = ToCamelCase(g_base_info_get_name(info));
    target->Set(String::NewSymbol(jsname), temp->GetFunction());

    delete[] jsname;
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
    
    String::Utf8Value fname(args.Callee()->Get(String::New("__fname__")));
    
    int l=strlen(*fname);
    char *ns, *fn;
    for(int i=0; i<l; i++) {
        if((*fname)[i] == ':') {
            
            ns = new char[i];
            for(int j=0; j<i; j++) {
                ns[j] = (*fname)[j];
            }
            ns[i] = '\0';
            
            fn = new char[l-i];
            for(int j=0; j<(l-i); j++) {
                fn[j] = (*fname)[i+j+1];
            }
            fn[l-i] = '\0';
            
            break;
        }
    }
    
    GIFunctionInfo *func = g_irepository_find_by_name(NamespaceLoader::repo, ns, fn);
    delete[] ns;
    delete[] fn;
    
    if(func) {
        return scope.Close(Func::Call(NULL, func, args));
    }
    else {
        return EXCEPTION("no such function");
    }
    
    return scope.Close(Undefined());
}


}
