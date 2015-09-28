#include <node.h>

#include <v8.h>

#include "util.h"
#include "namespace_loader.h"

NAN_MODULE_INIT(InitAll) {
    Nan::Set(target, Nan::New("load").ToLocalChecked(),
        Nan::GetFunction(Nan::New<v8::FunctionTemplate>(gir::NamespaceLoader::Load)).ToLocalChecked());
    Nan::Set(target, Nan::New("search_path").ToLocalChecked(),
        Nan::GetFunction(Nan::New<v8::FunctionTemplate>(gir::NamespaceLoader::SearchPath)).ToLocalChecked());
}

NODE_MODULE(girepository, InitAll)
