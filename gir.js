var gir = require('./build/default/girepository.node');

console.log(gir);
gir.init();

var gtk = exports.gtk = gir.load("Gtk");


gtk.HBox.__properties__;
gtk.HBox.__methods__;
gtk.HBox.__interfaces__;
gtk.HBox.__fields__;
var w = new gtk.HBox();

w.__call__("function_name");
w.__get_property__("property_name");
w.__get_interface__("Buildable");
w.__get_field__("name");

var x = new gtk.Box();
//console.log(gtk.HBox);

// FIXME: see object.cc
/*
w.ref();
w.unref();
*/
