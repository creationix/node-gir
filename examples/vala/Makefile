# This Example is forked from https://github.com/antono/vala-object

export LD_LIBRARY_PATH := $(shell pwd)
export GI_TYPELIB_PATH := $(shell pwd)

all: vala-object.so ValaObject-0.1.typelib

run: all run-node-js

run-node-js: all
	node test.node.js

c-source:
	valac --ccode object.vala

vala-object.so:
	valac \
		--enable-experimental    \
		-X -fPIC -X -shared      \
		--library=vala-object    \
		--gir=ValaObject-0.1.gir \
		-o vala-object.so        \
		object.vala

ValaObject-0.1.typelib:
	g-ir-compiler \
		--shared-library=vala-object.so \
		--output=ValaObject-0.1.typelib \
		ValaObject-0.1.gir

clean:
	rm -fr ValaObject-0.1.typelib vala-object.so ValaObject-0.1.gir vala-object.vapi object.c *~
