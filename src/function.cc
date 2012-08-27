#include "function.h"
#include "arguments.h"
#include "util.h"

#include <vector>

using namespace v8;

namespace gir {

static GIArgument *_gir_gi_argument_new(GObject *obj, int length) 
{
    if (length < 1)
        return NULL;

    GIArgument *args = g_new0(GIArgument, length);
    for (int i = 0; i < length; i++) {
        args[i].v_string = NULL;
    }

    if (obj != NULL) 
        args[0].v_pointer = obj;

    return args;
}

static void _gir_gi_argument_free(GIArgument *args, int length)
{
    if (length < 1)
        return;

    for (int i = 0; i < length; i++) {
        //g_free(args[i].v_string); FIXME, it has to be freed
    }
    g_free(args);
    args = NULL;
}

/* Check that the function is called with the correct number of arguments.
 * Returns a newly allocated exception message string on failure, or NULL on success. */
char * checkNumberOfArguments(GIFunctionInfo *info, const Arguments &args,
                              int *in_arguments_count, int *out_arguments_count) {

    int in_argc = *in_arguments_count;
    int out_argc = *out_arguments_count;

    const int l = g_callable_info_get_n_args(info);
    int required_arguments = l;
    int optional_arguments = 0;

    // Compute number of required arguments
    // Any out and error argument should be implicit
    for (int i=0; i<l; i++) {
        GIArgInfo *arg = g_callable_info_get_arg(info, i);
        GITypeInfo *arg_type_info = g_arg_info_get_type(arg);

        const GIDirection dir = g_arg_info_get_direction(arg);
        if(dir == GI_DIRECTION_IN) {
            in_argc++;
        }
        else if(dir == GI_DIRECTION_OUT) {
            out_argc++;
        }
        else {
            out_argc++;
            in_argc++;
        }
        debug_printf("%s %s (%d) \n",
                g_type_tag_to_string(g_type_info_get_tag(arg_type_info)),
                g_base_info_get_name(arg),
                dir);

        const bool is_optional = g_arg_info_is_optional(arg);
        const bool may_be_null = g_arg_info_may_be_null(arg);
        if (g_type_info_get_tag(arg_type_info) == GI_TYPE_TAG_ERROR
                || g_arg_info_get_direction(arg) == GI_DIRECTION_OUT
                || is_optional == TRUE
                || may_be_null == TRUE ) {
            required_arguments--;
        }

        if (is_optional)
            optional_arguments++;

        if (may_be_null)
            optional_arguments++;

        g_base_info_unref(arg_type_info);
        g_base_info_unref(arg);
    }

    *in_arguments_count = in_argc;
    *out_arguments_count = out_argc;

    if (args.Length() != required_arguments) {
        if (args.Length() != CLAMP(args.Length(), required_arguments, required_arguments + optional_arguments)) {
            char *exc_msg = g_strdup_printf("Invalid number of arguments. Expected %d, got %d", required_arguments, args.Length());
            return exc_msg;
        }
    }
    return NULL;
}


v8::Handle<v8::Value> Func::CallAndGetPtr(GObject *obj, GIFunctionInfo *info, const Arguments &args, bool ignore_function_name, GIArgument *retval, GITypeInfo **returned_type_info, gint *returned_array_length) {

    if(g_function_info_get_flags(info) == GI_FUNCTION_IS_CONSTRUCTOR) {
        // rly not sure about this
        debug_printf("constructor! returns %s\n", g_type_tag_to_string( g_type_info_get_tag( g_callable_info_get_return_type(info) ) ));
        obj = NULL;
    }

    *returned_array_length = -1;
    *returned_type_info = g_callable_info_get_return_type(info);
    const int offset_ = (obj != NULL) ? 1 : 0;
    const int l = g_callable_info_get_n_args(info);
    int in_argc_c_length = offset_, out_argc_c_length = 0;

    // Verify that function is called with right number of arguments
    char *exc_msg = checkNumberOfArguments(info, args, &in_argc_c_length, &out_argc_c_length);
    if (exc_msg) {
        return ThrowException(Exception::TypeError(String::New(exc_msg)));
    }
    debug_printf("(%d) in_argc_c_length is %d, out_argc_c_length is %d, offset is %d\n", l, in_argc_c_length, out_argc_c_length, offset_);

    GIArgument *in_args = _gir_gi_argument_new(obj, in_argc_c_length);
    GIArgument *out_args = _gir_gi_argument_new(NULL, out_argc_c_length);
    gpointer *out_args_c = NULL;
    if (out_argc_c_length > 0) {
        out_args_c = g_new0(gpointer, out_argc_c_length);
    }

    const int returned_array_real_pos = g_type_info_get_array_length(*returned_type_info);
    int returned_array_pos = -1;
    int in_c = offset_, out_c = 0;
    for(int i=0; i<l; i++) {
        if (returned_array_real_pos == i) {
            returned_array_pos = out_c;
        }
        /* Ignore function name in arguments:
         * o.__call__("func_name", args) VS o.func_name(args) */
        const int real_arg_idx = ignore_function_name ? i : i + offset_;
        GIArgInfo *arg = g_callable_info_get_arg(info, i);
        GIDirection dir = g_arg_info_get_direction(arg);
        if(dir == GI_DIRECTION_IN || dir == GI_DIRECTION_INOUT) {
            if(!Args::ToGType(args[real_arg_idx], &in_args[in_c], arg, NULL, FALSE)) {
                return BAD_ARGS("IN arguments conversion failed");
            }
            //printf("IN ARG (%d) '%s' \n", in_c, in_args[in_c].v_string);
            in_c++;
        }
        if(dir == GI_DIRECTION_OUT || dir == GI_DIRECTION_INOUT) { 
            if(!Args::ToGType(args[real_arg_idx], &out_args[out_c], arg, NULL, TRUE)) {
                return BAD_ARGS("OUT arguments conversion failed");
            }
            out_c++;
        }
        g_base_info_unref(arg);
    }
        
    GError *error = NULL;
   
    // Initilize out arguments pointers
    int n;
    for (n = 0; n < out_c; n++) {
        out_args[n].v_pointer = &out_args_c[n];
        //printf ("OUT ARG[%d](%d) (%d) '%s' \n", n, out_argc_c_length, out_args[n].v_uint32, "" /* out_args[n].v_string */);
    }
    for (n = 0; n < in_argc_c_length; n++) {
        //printf ("IN ARG[%d](%d) (%s) \n", n, in_argc_c_length, in_args[n].v_string);
    }

    gboolean invoked = g_function_info_invoke(info, in_args, in_argc_c_length, out_args, out_argc_c_length, retval, &error);

    // Free possible allocated strings
    for (n = 0; n < out_c; n++) {
        //printf ("OUT ARG[%d](%d) (%d) uint (%d) \n", n, out_argc_c_length, out_args[n].v_pointer, (guint) out_args_c[n]);
        //g_free(out_args[n].v_string);
    }

    if(!invoked) {
        _gir_gi_argument_free(in_args, in_argc_c_length);
        _gir_gi_argument_free(out_args, out_argc_c_length);
        g_free(out_args_c);
        return EXCEPTION(error->message);
    }

    // TODO, set out values

    GITypeTag tag = g_type_info_get_tag(*returned_type_info);
    // Set returned array length
    if (tag == GI_TYPE_TAG_ARRAY) {
        if (returned_array_pos > -1) {
            *returned_array_length = (int) GPOINTER_TO_INT(out_args_c[returned_array_pos]);
        }
    }

    _gir_gi_argument_free(in_args, in_argc_c_length);
    _gir_gi_argument_free(out_args, out_argc_c_length);
    g_free(out_args_c);

    return Null();
}

Handle<Value> Func::Call(GObject *obj, GIFunctionInfo *info, const Arguments &args, bool ignore_function_name) {

    if(g_function_info_get_flags(info) == GI_FUNCTION_IS_CONSTRUCTOR) {
        // rly not sure about this
        debug_printf("constructor! returns %s\n", g_type_tag_to_string( g_type_info_get_tag( g_callable_info_get_return_type(info) ) ));
        obj = NULL;
    }
   
    GIArgument retval;
    GITypeInfo *returned_type_info;
    gint returned_array_length;

    CallAndGetPtr(obj, info, args, ignore_function_name, &retval, &returned_type_info, &returned_array_length); 
    Handle<Value> return_value = Args::FromGType(&retval, returned_type_info, returned_array_length);

    if (returned_type_info != NULL)
        g_base_info_unref(returned_type_info);

    /* TODO, free GIArgument ? */

    return return_value;
}

}
