#include "init.h"
#include <node.h>

#include <v8.h>

#include "util.h"
#include "namespace_loader.h"

extern "C" void init (v8::Handle<v8::Object> target) {
    v8::HandleScope scope;

    NODE_SET_METHOD(target, "init", init);
    gir::NamespaceLoader::Initialize(target);
}
