var gir = require('./build/default/girepository.node');

console.log(gir);
gir.init();

var gtk = exports.gtk = gir.load("Gtk");


gtk.HBox.__properties__;
gtk.HBox.__methods__;
var w = new gtk.HBox();

w.__call__("function_name");
w.__get_property__("property_name");

// FIXME: see object.cc
/*
w.ref();
w.unref();
*/
