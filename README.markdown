# node-gir

Node-gir is Node.js bindings to [GObject Introspection](https://live.gnome.org/GObjectIntrospection/) making it possible to make automatic and dynamic calls to any library that has GI annotations installed. This includes most libraries from the [GNOME project](http://developer.gnome.org/).

This will make it possible to script a GNOME desktop system entirely from node
much in the way it's done today with Seed, GJS or pygtk. It also allows using GNOME libraries in Node.js applications.

## Installation

You need GObject Introspection library to be installed. On a Debian-like system this would be handled by:

    $ sudo apt-get install libgirepository1.0-dev

Then just build node-gir with:

    $ npm install

## Testing

The node-gir repository comes with a set of tests that utilize the Midgard2 library to test against. You need also that installed, and then run:

    $ npm test

Travis is used for Continous Integration:

[![Build Status](https://secure.travis-ci.org/piotras/node-gir.png?branch=master)](http://travis-ci.org/piotras/node-gir)

## Architecture

The following graph shows all the parts and how they work together.  The only
missing part is node bindings to libgirepository.  Hence this project.

    BUILD TIME:

              +-----------------------------------------------------------+
              |   foo.c                                                   |
              |   foo.h                                                   |
              |                                                           |
              | Library sources, with type annotations                    |
              +-----------------------------------------------------------+
                  |                                      |
                 gcc                                 g-ir-scanner
                  |                                      |
                  |                                      V
                  |                              +------------------------+
                  |                              |    Foo.gir             |
                  |                              |                        |
                  |                              |    <GI-name>.gir       |
                  |                              |                        |
                  |                              | XML file               |
                  |                              |                        |
                  |                              | Invocation information |
                  |                              | Required .gir files    |
                  |                              | API docs               |
                  |                              |                        |
                  |                              +------------------------+
                  |                                          |
                  |                                     g-ir-compiler
                  |                                          |
      DEPLOYMENT TIME:                                       |
                  |                                          |
                  V                                          V
            +-----------------------------+      +---------------------------+
            |   libfoo.so                 |      |    Foo.typelib            |
            |                             |      |                           |
            |                             |      | Binary version of the     |
            |  ELF file                   |      | invocation info and       |
            |                             |      | required .typelib files   |
            |  Machine code, plus         |      +---------------------------+
            | dynamic linkage information |                  A
            | (DWARF debug data, etc)     |                  |
            +-----------------------------+                  |
                         A                                   |
                         |                       +---------------------------+
                         |                       |  libgirepository.so       |
                    +-----------+                |                           |
                    | libffi.so |                |  Can read typelibs and    |
                    |           |                |  present them in a        |
                    +-----------+                |  libffi-based way         |
                         A                       |                           |
                         |                       +---------------------------+
                         |                                  A
                         |                                  |
                         |                          +------------+
                         +--------------------------|  node-gir  |
                                                    |            |
                                         +--------->+------------+
                                         |
                               +------------------+
                               |      NodeJS      |
                               +------------------+

## Why not use Seed or GJS

Because they are nice, but not what I'm looking for.  Node is really popular and
it would be nice to be able to use it for desktop tools and applications.  

## Implementation Notes

Here are some links and notes as I try to figure out how to do this.

 - <http://live.gnome.org/GObjectIntrospection/HowToWriteALanguageBinding>
 - <http://developer.gnome.org/gi/unstable/gi-girepository.html>

## API Ideas

Some of these ideas will go in this binding and some will go in nice wrappers
that use it.  I'll know more as we progress.

 - Use `camelCase` for methods that are bound to look JavaScripty.
 - Use `.on(name, callback)` to attach signals.
 - Keep the same constructor style used by Seed and GJS
 - Make the module system as node-like as possible.

## Things which work

 - All classes get created
 - classes get inherited
 - interface methods are inherited
 - A class has lists of all its properties, methods, signals, vfuncs and fields
 - C structures are propagated as objects (fields are properties)
 - Both methods and static method can be called
 - You can create a class
 - functions can be called (but it does not work so well with 'out' arguments which should be set as returned value)
 - GError is propagated as generic exception
 - property values can be set/get
 - events can be watched
 - flags, enums etc are set

## Things which dont work (correct)

 - Conversion between a v8 value and a GValue/GArgument is veeeery buggy (but everything needs it so most things are buggy)
 - No support for libev/libuv; glib is using its own stuff (gst.main())
 - There is no good way to delete an object (memory management sucks at all)
 - types/function.cc need a rewrite
 - GError should be propagated as derived classes depending on GError domain
