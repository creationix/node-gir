#include <v8.h>
#include <node.h>

#include <glib.h>
#include <glib-object.h>
#include <girepository.h>

using namespace node;
using namespace v8;

void test(void)
{
    GIRepository *repository;
    GError *error = NULL;
    GIBaseInfo *base_info;
    GIArgument in_args[5];
    GIArgument retval;

    g_type_init();

    repository = g_irepository_get_default();
    g_irepository_require(
      repository,
      "GLib",
      "2.0",
      (GIRepositoryLoadFlags)0, 
      &error);
    if (error) {
      g_error("ERROR: %s\n", error->message);
      return;
    }

    base_info = g_irepository_find_by_name(repository, "GLib", "assertion_message");
    if (!base_info) {
      g_error("ERROR: %s\n", "Could not find GLib.warn_message");
      return;
    }

    in_args[0].v_pointer = (void*)"domain";
    in_args[1].v_pointer = (void*)"glib-print.c";
    in_args[2].v_pointer = (void*)"30";
    in_args[3].v_pointer = (void*)"main";
    in_args[4].v_pointer = (void*)"hello world";

    if (!g_function_info_invoke ((GIFunctionInfo *)base_info, (const GIArgument*)&in_args, 5, NULL, 0, &retval, &error)) {
      g_error("ERROR: %s\n", error->message);
      return;
    }

    g_base_info_unref (base_info);

}


static Handle<Value> prepend_search_path(const Arguments& args)
{
  HandleScope scope;
  
  if (args.Length() != 1) {
    return ThrowException(Exception::TypeError(String::New("One string argument required")));
  }
  String::Utf8Value path(args[0]->ToString());
  
  // TODO: check to see if path needs to be deallocated or copied
  g_irepository_prepend_search_path(*path);
  return Undefined();
}

static Handle<Value> get_search_path(const Arguments& args)
{
  HandleScope scope;
  GSList *search_path;
  
  search_path = g_irepository_get_search_path();
  printf("%x\n", (int)search_path);
  while (search_path) {
    printf("%x\n", (int)search_path);
    search_path = search_path->next;
  }
//  g_list_free_full

  return Undefined();

}

static Handle<Value> g_type_init(const Arguments& args)
{
  HandleScope scope;
//  g_type_init();
  test();
  return Undefined();
}


extern "C" {
  static void init(Handle<Object> target)
  {
    NODE_SET_METHOD(target, "gTypeInit", g_type_init);
    NODE_SET_METHOD(target, "getSearchPath", get_search_path);
    NODE_SET_METHOD(target, "prependSearchPath", prepend_search_path);
  }

  NODE_MODULE(node_gir_native, init);
}
