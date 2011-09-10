#include "function.h"
#include "arguments.h"
#include "util.h"

#include <vector>

using namespace v8;

namespace gir {

Handle<Value> Func::Call(GObject *obj, GIFunctionInfo *info, const Arguments &args) {
    int offset_ = 0;
    if(obj != NULL) {
        offset_ = 1;
    }
    
    int l = g_callable_info_get_n_args(info);
    int in_args_c = offset_, out_args_c = 0;
    for(int i=0; i<l; i++) {
        GIArgInfo *arg = g_callable_info_get_arg(info, i);
        GIDirection dir = g_arg_info_get_direction(arg);
        if(dir == GI_DIRECTION_IN) {
            in_args_c++;
        }
        else if(dir == GI_DIRECTION_OUT) {
            out_args_c++;
        }
        else {
            out_args_c++;
            in_args_c++;
        }
        g_base_info_unref(arg);
    }
    printf("in_args_c is %d, out_args_c is %d, offest ist %d\n", in_args_c, out_args_c, offset_);
    
    GIArgument in_args[in_args_c];
    GIArgument out_args[out_args_c];
    
    int in_c = offset_, out_c = 0;
    for(int i=0; i<l; i++) {
        GIArgInfo *arg = g_callable_info_get_arg(info, i);
        GIDirection dir = g_arg_info_get_direction(arg);
        if(dir == GI_DIRECTION_IN || dir == GI_DIRECTION_INOUT) {
            if(!Args::ToGType(args[i+offset_], &in_args[in_c], arg)) {
                return BAD_ARGS();
            }
            in_c++;
        }
        if(dir == GI_DIRECTION_OUT || dir == GI_DIRECTION_INOUT) {
            if(!Args::ToGType(args[i+offset_], &(out_args[out_c]), arg)) {
                return BAD_ARGS();
            }
            out_c++;
        }
        g_base_info_unref(arg);
    }
    
    in_args[0].v_pointer = obj;
    
    GError *error = NULL;
    GIArgument retval;
    
    const GIArgument *invoke_in_args = NULL;
    const GIArgument *invoke_out_args = NULL;
    if(in_c > 0) {
        invoke_in_args = (const GIArgument*)&in_args;
    }
    if(out_c > 0) {
        invoke_out_args = (const GIArgument*)&out_args;
    }

    if(!g_function_info_invoke(info, invoke_in_args, in_args_c, invoke_out_args, out_args_c, &retval, &error)) {
        return EXCEPTION(error->message);
    }
    else {
        //return retval and all out_args
        return Args::FromGType(&retval, g_callable_info_get_return_type(info));
    }
}

/*
Handle<Value> ValueToArgs::CallFunc(GObject *obj, GIFunctionInfo *info, const Arguments &args) {
    int l = g_callable_info_get_n_args(info);
    int in_args_c = 1, out_args_c = 0;
    for(int i=0; i<l; i++) {
        GIArgInfo *arg = g_callable_info_get_arg(info, i);
        GIDirection dir = g_arg_info_get_direction(arg);
        if(dir == GI_DIRECTION_IN) {
            in_args_c++;
        }
        else if(dir == GI_DIRECTION_OUT) {
            out_args_c++;
        }
        else {
            out_args_c++;
            in_args_c++;
        }
        g_base_info_unref(arg);
    }
    
    GIArgument in_args[in_args_c];
    GIArgument out_args[out_args_c];
    
    if(args.Length()-1 < l) {
        return EXCEPTION("too few arguments");
    }
    
    int in_c = 1, out_c = 0;
    for(int i=0; i<l; i++) {
        GIArgInfo *arg = g_callable_info_get_arg(info, i);
        GIDirection dir = g_arg_info_get_direction(arg);
        if(dir == GI_DIRECTION_IN || dir == GI_DIRECTION_INOUT) {
            if(!Convert(args[i+1], &in_args[in_c], arg)) {
                return BAD_ARGS();
            }
            in_c++;
        }
        if(dir == GI_DIRECTION_OUT || dir == GI_DIRECTION_INOUT) {
            if(!Convert(args[i+1], &(out_args[out_c]), arg)) {
                return BAD_ARGS();
            }
            out_c++;
        }
        g_base_info_unref(arg);
    }
    
    in_args[0].v_pointer = obj;
    
    GError *error = NULL;
    GIArgument retval;
    
    const GIArgument *invoke_in_args = NULL;
    const GIArgument *invoke_out_args = NULL;
    if(in_c > 0) {
        invoke_in_args = (const GIArgument*)&in_args;
    }
    if(out_c > 0) {
        invoke_out_args = (const GIArgument*)&out_args;
    }

    if(!g_function_info_invoke(info, invoke_in_args, in_args_c, invoke_out_args, out_args_c, &retval, &error)) {
        return EXCEPTION(error->message);
    }
    else {
        //return retval and all out_args
        return ArgToValue(&retval, g_callable_info_get_return_type(info));
    }
}*/

}
