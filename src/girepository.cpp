#include <v8.h>
#include <node.h>

#include <glib.h>
#include <glib-object.h>
#include <girepository.h>

using namespace node;
using namespace v8;


static Handle<Value> g_type_init(const Arguments& args)
{
  g_type_init();
  return Undefined();
}

static Handle<Value> gir_prepend_search_path(const Arguments& args)
{
  HandleScope scope;
  String::Utf8Value path(args[0]->ToString());
  // TODO: check to see if path needs to be deallocated or copied
  g_irepository_prepend_search_path(*path);
  return Undefined();
}

static Handle<Value> gir_get_search_path(const Arguments& args)
{
  GSList* search_path = g_irepository_get_search_path();
  // TODO: get this returned as an V8 array
  return ThrowException(Exception::TypeError(String::New("TODO: finish gir_get_search_path")));
}

static Handle<Value> gir_get_default(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_get_default")));
}

static Handle<Value> gir_load_typelib(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_load_typelib")));
}


static Handle<Value> gir_is_registered(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_is_registered")));
}


static Handle<Value> gir_find_by_name(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_find_by_name")));
}


static Handle<Value> gir_require(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_require")));
}


static Handle<Value> gir_require_private(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_require_private")));
}


static Handle<Value> gir_get_dependencies(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_get_dependencies")));
}


static Handle<Value> gir_get_loaded_namespaces(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_get_loaded_namespaces")));
}


static Handle<Value> gir_find_by_gtype(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_find_by_gtype")));
}


static Handle<Value> gir_get_n_infos(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_get_n_infos")));
}


static Handle<Value> gir_get_info(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_get_info")));
}


static Handle<Value> gir_get_typelib_path(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_get_typelib_path")));
}


static Handle<Value> gir_get_shared_library(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_get_shared_library")));
}


static Handle<Value> gir_get_version(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_get_version")));
}


static Handle<Value> gir_get_option_group(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_get_option_group")));
}


static Handle<Value> gir_get_c_prefix(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_get_c_prefix")));
}


static Handle<Value> gir_dump(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_dump")));
}


static Handle<Value> gir_enumerate_versions(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind gir_enumerate_versions")));
}


static Handle<Value> g_typelib_new_from_memory(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind g_typelib_new_from_memory")));
}


static Handle<Value> g_typelib_new_from_const_memory(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind g_typelib_new_from_const_memory")));
}


static Handle<Value> g_typelib_new_from_mapped_file(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind g_typelib_new_from_mapped_file")));
}


static Handle<Value> g_typelib_free(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind g_typelib_free")));
}


static Handle<Value> g_typelib_symbol(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind g_typelib_symbol")));
}


static Handle<Value> g_typelib_get_namespace(const Arguments& args)
{
  return ThrowException(Exception::TypeError(String::New("TODO: bind g_typelib_get_namespace")));
}


extern "C" {
  static void init(Handle<Object> target)
  {
    // TODO: Add GIRepository constructor
    NODE_SET_METHOD(target, "gTypeInit", g_type_init);
    NODE_SET_METHOD(target, "getDefault", gir_get_default);
    NODE_SET_METHOD(target, "prependSearchPath", gir_prepend_search_path);
    NODE_SET_METHOD(target, "getSearchPath", gir_get_search_path);
    NODE_SET_METHOD(target, "loadTypelib", gir_load_typelib);
    NODE_SET_METHOD(target, "isRegistered", gir_is_registered);
    NODE_SET_METHOD(target, "findByName", gir_find_by_name);
    NODE_SET_METHOD(target, "require", gir_require);
    NODE_SET_METHOD(target, "requirePrivate", gir_require_private);
    NODE_SET_METHOD(target, "getDependencies", gir_get_dependencies);
    NODE_SET_METHOD(target, "getLoadedNamespaces", gir_get_loaded_namespaces);
    NODE_SET_METHOD(target, "findByGtype", gir_find_by_gtype);
    NODE_SET_METHOD(target, "getNInfos", gir_get_n_infos);
    NODE_SET_METHOD(target, "getInfo", gir_get_info);
    NODE_SET_METHOD(target, "getTypelibPath", gir_get_typelib_path);
    NODE_SET_METHOD(target, "getSharedLibrary", gir_get_shared_library);
    NODE_SET_METHOD(target, "getVersion", gir_get_version);
    NODE_SET_METHOD(target, "getOptionGroup", gir_get_option_group);
    NODE_SET_METHOD(target, "getCPrefix", gir_get_c_prefix);
    NODE_SET_METHOD(target, "dump", gir_dump);
    NODE_SET_METHOD(target, "enumerateVersions", gir_enumerate_versions);
    NODE_SET_METHOD(target, "newFromMemory", g_typelib_new_from_memory);
    NODE_SET_METHOD(target, "newFromConstMemory", g_typelib_new_from_const_memory);
    NODE_SET_METHOD(target, "newFromMappedFile", g_typelib_new_from_mapped_file);
    NODE_SET_METHOD(target, "free", g_typelib_free);
    NODE_SET_METHOD(target, "symbol", g_typelib_symbol);
    NODE_SET_METHOD(target, "getNamespace", g_typelib_get_namespace);
  }
  NODE_MODULE(girepository, init);
}
