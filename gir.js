var gir = require('./build/default/girepository.node');

console.log(gir);
gir.init();

var gtk = exports.gtk = gir.load("Gtk");


/*
gtk.HBox.__properties__;
gtk.HBox.__methods__;
gtk.HBox.__interfaces__;
gtk.HBox.__fields__;
var w = new gtk.HBox();
*/

/*
console.log(w.__call__("get_spacing"));
console.log(w.__call__("set_spacing", 22));
console.log(w.__call__("get_spacing"));
*/

/*
console.log(w.__call__("get_name"));
console.log(w.__call__("set_name", "test"));
console.log(w.__call__("get_name"));
*/

gtk.init(0, null);


var win = new gtk.Window();
var button = new gtk.Button();

win.__set_property__("title", "hallo, welt!");
win.__call__("set_border_width", 10);

button.__call__("set_label", "hallo, welt!");


win.__call__("add", button);
console.log(typeof button.__get_property__("parent"));
win.__call__("show");

gtk.main();
//setInterval(function() { }, 10000);

/*
w.__get_property__("property_name");
w.__get_interface__("Buildable");
w.__get_field__("name");

var x = new gtk.HBox();
console.log(x.__watch_signal__("show"));
console.log(x instanceof gtk.HBox);
console.log(x instanceof gtk.Box);

//console.log(gtk.HBox.__methods__);
*/
