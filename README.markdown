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
that use it. I'll know more as we progress.

 - Keep the same constructor style used by Seed and GJS.
 - Make the module system as node-like as possible.
 - Identify static objects/members versus instance objects/members.

## Things which work

 - All classes get created. Classes get inherited.
 - A class has lists of all its properties, methods, signals, vfuncs, and fields. Use inspector example to print a complete dump of object members prior to an object instance. Object instances may vary from the prototypes.
 - Properties, methods, and vfuncs use `conventionalCamelCasing` for names. They are wired into the `otherwise\_unconventional\_underscores` and `dashes-in-names` counterparts on the back end.
 - Events can be watched. Signals are associated with `EventEmitter` object provided by Node. Use `.on('signal-name', callback)` to attach signals, or use shortcut syntax `onSignalName(callback)`.
 - Methods can be called.
 - Property values can be set/get. JavaScript getters/setters are in-place to ensure that properties are queried properly.
 - Flags, Enums, etc., are set.

## Things which dont work (correct)

 - Conversion between a v8 value and a GValue/GArgument is veeeery buggy (but everything needs it so most things are buggy)
 - The API is becoming more consistent (classes just have \_\_call\_method\_\_, \_\_get\_prroperty\_\_ etc
   but the namespace has all methods [ gst.main(), gst.mainQuit()]
 - No support for libev/libuv; glib is using its own stuff (gst.main())
 - There is no good way to delete an object (memory management sucks at all)
 - You can't pass construction parameters to g\_object\_new
 - Only the GObject and Function type is implementet yet (left are GIInterfaceInfo and GIStructInfo)
 - types/function.cc need a rewrite
 - Static members fail, usually with TypeError. This probably means the actual objects need members to be set in addition to the prototypes, but there is no way to tell which is static and which is instance.
 - Some objects that are returned by methods (and possibly properties) do not have the `\_\_call\_\_`, `\_\_get\_property\_\_`, `\_\_set\_property\_\_`, and friends prototype members. This produces 'Error: bad arguments' on the JavaScript consumer.
