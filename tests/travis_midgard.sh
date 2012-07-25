#!/bin/bash

# Options
MIDGARD_LIBS_VERSION=10.05.7
MIDGARD_EXT_VERSION=ratatoskr

# Install Midgard2 library dependencies 
sudo apt-get install -y dbus libglib2.0-dev libgda-4.0-4 libgda-4.0-dev libxml2-dev libdbus-1-dev libdbus-glib-1-dev gobject-introspection libgirepository1.0-dev

# Build Midgard2 core from recent tarball
wget -q https://github.com/midgardproject/midgard-core/tarball/${MIDGARD_EXT_VERSION}
tar -xzf ratatoskr
sh -c "cd midgardproject-midgard-core-*&&./autogen.sh --prefix=/usr; make; sudo make install"
rm -f ${MIDGARD_EXT_VERSION}
