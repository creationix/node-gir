var gir = require('./build/default/girepository.node');

console.log(gir);
gir.init();

var gtk = exports.gtk = gir.load("Gtk");

var w = new gtk.HBox();

for(var k in w) {
    console.log(k, w[k]);
}

w.__call__("set_spacing");
w.__get_property__("name_asd");
