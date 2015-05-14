#ifndef GIR_INIT_H
#define GIR_INIT_H

#include <v8.h>
#include <glib.h>
#include <girepository.h>
#include "nan.h"

NAN_METHOD(init) {
	NanScope();
    NanReturnUndefined();
}

#endif
