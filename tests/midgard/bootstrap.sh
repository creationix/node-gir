#!/bin/sh

npm install vows
npm install gir
# Create symlink in case npm is too old
ln -s default node_modules/gir/build/Release
