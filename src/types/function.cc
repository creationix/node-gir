#include "function.h"
#include "../namespace_loader.h"
#include "../util.h"
#include "../function.h"

#include <string.h>
#include <node.h>

using namespace v8;

namespace gir {

void GIRFunction::Initialize(Handle<Object> target, GIObjectInfo *info) 
{
    const char *func_name = g_base_info_get_name(info);
    // Create new function
    Local<FunctionTemplate> temp = FunctionTemplate::New(Execute);
    // Set name
    temp->GetFunction()->SetName(String::New(func_name));
    // Create external to hold GIBaseInfo and set it
    v8::Handle<v8::External> info_ptr = v8::External::New((void*)g_base_info_ref(info));
    temp->GetFunction()->SetHiddenValue(String::New("GIInfo"), info_ptr);
    // Set symbol
    target->Set(String::NewSymbol(func_name), temp->GetFunction());
}

Handle<Value> GIRFunction::Execute(const Arguments &args) 
{
    HandleScope scope;
    // Get GIFunctionInfo pointer    
    v8::Handle<v8::External> info_ptr =
        v8::Handle<v8::External>::Cast(args.Callee()->GetHiddenValue(String::New("GIInfo")));
    GIBaseInfo *func  = (GIBaseInfo*) info_ptr->Value();

    debug_printf("EXECUTE namespace: '%s',  name: '%s', symbol: '%s' \n", 
            g_base_info_get_namespace(func),
            g_base_info_get_name(func),
            g_function_info_get_symbol(func));

    if(func) {
        return scope.Close(Func::Call(NULL, func, args, TRUE));
    }
    else {
        return EXCEPTION("no such function");
    }
    
    return scope.Close(Undefined());
}

}
