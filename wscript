import Options
from os import unlink, symlink, popen
from os.path import exists

APPNAME = "node_gir_native"
VERSION = "0.0.1"
srcdir = "."
blddir = "build"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  conf.check_cfg(package='gobject-introspection-1.0', uselib_store='GIREPOSITORY', args='--cflags --libs')
  conf.check_cfg(package='glib', uselib_store='GLIB', args='--cflags --libs')

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "node_gir_native"
  obj.source = "src/node_gir.cpp"
  obj.cxxflags = ["-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE"]
  obj.uselib = "GIREPOSITORY GLIB"

def shutdown():
  if Options.commands['clean']:
    if exists('node_gir_native.node'): unlink('node_gir_native.node')

