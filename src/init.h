#ifndef GIR_INIT_H
#define GIR_INIT_H

#include <v8.h>
#include <glib.h>
#include <girepository.h>

v8::Handle<v8::Value> init(const v8::Arguments &args) {
    v8::HandleScope scope;
    g_type_init();
    return scope.Close(v8::Undefined());
}

#endif
