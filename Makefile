NODE_JS = $(if $(shell test -f /usr/bin/nodejs && echo "true"),nodejs,node)
NODE_BLD = node-waf
NODE_LIB_PATH = ~/.node_libraries

BASE = .
INSTALL_PATH = $(NODE_LIB_PATH)/node_gir_native

all: build_bindings

update: clean config build_bindings uninstall install

config:
	@@$(BASE)/configure

build_bindings:
	@@$(NODE_BLD) build

install:
	@@mkdir -p $(INSTALL_PATH)
	@@mkdir -p $(INSTALL_PATH)/build/default

	@@cp -f $(BASE)/build/default/node_gir_native.node $(INSTALL_PATH)/build/default/node_gir_native.node
	@@cp -f $(BASE)/package.json $(INSTALL_PATH)/

	@@echo "Installed to $(INSTALL_PATH)"

uninstall:
	@@rm -rf $(INSTALL_PATH)
	@@echo "Uninstalled from $(INSTALL_PATH)"

clean:
	@@rm -rf $(BASE)/build

unittest:
	@@$(NODE_JS) $(BASE)/test/index.js test

lint:
	@@$(NODE_JS) $(BASE)/util/hint-check.js
