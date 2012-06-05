import Options
from os import unlink, symlink, popen
from os.path import exists

srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  conf.check_cfg(package='gobject-introspection-1.0', uselib_store='GIREPOSITORY', args='--cflags --libs')
  conf.check_cfg(package='glib-2.0', uselib_store='GLIB', args='--cflags --libs')

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "girepository"
  obj.source = [
    "src/init.cc",
    "src/util.cc",
    "src/namespace_loader.cc",
    "src/arguments.cc",
    "src/function.cc",
    "src/values.cc",
    "src/types/object.cc",
    "src/types/struct.cc",
    "src/types/function.cc"
  ]
  obj.cxxflags = ["-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE"]
  obj.uselib = "GIREPOSITORY GLIB"

def shutdown():
  if Options.commands['clean']:
    if exists('girepository.node'): unlink('girepository.node')

