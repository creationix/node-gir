#include "function_type.h"
#include "../namespace_loader.h"
#include "../util.h"
#include "../function.h"

#include <string.h>
#include <node.h>
#include <nan.h>

using namespace v8;

namespace gir {

void GIRFunction::Initialize(Handle<Object> target, GIObjectInfo *info) 
{
    const char *func_name = g_base_info_get_name(info);
    // Create new function
    Local<FunctionTemplate> temp = Nan::New<FunctionTemplate>(Execute);
    // Set name
    temp->GetFunction()->SetName(Nan::New<String>(func_name).ToLocalChecked());
    // Create external to hold GIBaseInfo and set it
    v8::Handle<v8::External> info_ptr = Nan::New<v8::External>((void*)g_base_info_ref(info));
    temp->GetFunction()->SetHiddenValue(Nan::New<String>("GIInfo").ToLocalChecked(), info_ptr);
    // Set symbol
    target->Set(Nan::New<String>(func_name).ToLocalChecked(), temp->GetFunction());
}

NAN_METHOD(GIRFunction::Execute) 
{
    // Get GIFunctionInfo pointer
    v8::Handle<v8::External> info_ptr =
        v8::Handle<v8::External>::Cast(info.Callee()->GetHiddenValue(Nan::New<String>("GIInfo").ToLocalChecked()));
    GIBaseInfo *func  = (GIBaseInfo*) info_ptr->Value();

    debug_printf("EXECUTE namespace: '%s',  name: '%s', symbol: '%s' \n", 
            g_base_info_get_namespace(func),
            g_base_info_get_name(func),
            g_function_info_get_symbol(func));

    if(func) {
        info.GetReturnValue().Set(Func::Call(NULL, func, info, TRUE));
    }
    else {
        Nan::ThrowError("no such function");
    }
    
    info.GetReturnValue().SetUndefined();
}

}
