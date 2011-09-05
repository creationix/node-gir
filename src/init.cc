#include "init.h"

#include <v8.h>

#include "util.h"
#include "namespace_loader.h"

extern "C" void init (v8::Handle<v8::Object> target) {
    v8::HandleScope scope;

    GIR_SET_METHOD(target, "init", init);
    gir::NamespaceLoader::Initialize(target);
}
