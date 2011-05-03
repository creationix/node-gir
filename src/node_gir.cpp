#include <v8.h>
#include <node.h>

#include <glib.h>
#include <glib-object.h>
#include <girepository.h>

using namespace node;
using namespace v8;

extern "C" {
  static void init(Handle<Object> target)
  {
//    NODE_SET_METHOD(target, "createWindow", create_window);
  }

  NODE_MODULE(node_gir_native, init);
}
