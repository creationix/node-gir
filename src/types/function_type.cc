#include "function_type.h"
#include "../namespace_loader.h"
#include "../util.h"
#include "../function.h"

#include <string.h>
#include <node.h>
#include "nan.h"

using namespace v8;

namespace gir {

void GIRFunction::Initialize(Handle<Object> target, GIObjectInfo *info) 
{
    const char *func_name = g_base_info_get_name(info);
    // Create new function
    Local<FunctionTemplate> temp = NanNew<FunctionTemplate>(Execute);
    // Set name
    temp->GetFunction()->SetName(NanNew<String>(func_name));
    // Create external to hold GIBaseInfo and set it
    v8::Handle<v8::External> info_ptr = NanNew<v8::External>((void*)g_base_info_ref(info));
    temp->GetFunction()->SetHiddenValue(NanNew<String>("GIInfo"), info_ptr);
    // Set symbol
    target->Set(NanNew<String>(func_name), temp->GetFunction());
}

NAN_METHOD(GIRFunction::Execute) 
{
    NanScope();
    // Get GIFunctionInfo pointer    
    v8::Handle<v8::External> info_ptr =
        v8::Handle<v8::External>::Cast(args.Callee()->GetHiddenValue(NanNew<String>("GIInfo")));
    GIBaseInfo *func  = (GIBaseInfo*) info_ptr->Value();

    debug_printf("EXECUTE namespace: '%s',  name: '%s', symbol: '%s' \n", 
            g_base_info_get_namespace(func),
            g_base_info_get_name(func),
            g_function_info_get_symbol(func));

    if(func) {
        NanReturnValue(Func::Call(NULL, func, args, TRUE));
    }
    else {
        NanThrowError("no such function");
    }
    
    NanReturnUndefined();
}

}
