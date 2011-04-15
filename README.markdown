# node-gir

Node-gir is node bindings to the girepository library making it possible to make
automatic and dynamic calls to any library that has GI annotations installed.  

This will make it possible to script a gnome desktop system entirely from node
much in the way it's done today with Seed, GJS or pygtk.

## Arcitecture

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
                    +------------+                          |
                    |  node-ffi  |                  +------------+
                    |            |<------+--------->|  node-gir  |
                    +------------+       |          |            |
                                         |          +------------+
                                         |
                               +------------------+
                               |      NodeJS      |
                               +------------------+

## Why not use Seed or GJS

Because they are nice, but not what I'm looking for.  Node is really popular and
it would be nice to be able to use it for desktop tools and applications.

## Plea for Help

I am not a C or C++ coder and have never once made node bindings.  I want this
library bad enough that I will learn what's needed, but it will take a while
since it's not my day job.  Anyone with experience in this stuff that would like
to lend a hand would be much appreciated.

## API Ideas

Some of these ideas will go in this binding and some will go in nice wrappers
that use it.  I'll know more as we progress.

 - Use `camelCase` for methods that are bound to look JavaScripty.
 - Use `.on(name, callback)` to attach signals.
 - Keep the same constructor style used by Seed and GJS
 - Make the module system as node-like as possible.

