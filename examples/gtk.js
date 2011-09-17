var gir = require("../gir"),
    gtk = gir.load("Gtk");

gtk.init(0);

var win = new gtk.Window({type: gtk.WindowType.toplevel, title:"trololol"});
var button = new gtk.Button();

win.__call__("set_border_width", 10);

button.__call__("set_label", "hallo, welt!");

win.__call__("add", button);
win.__call__("show_all");


var w2 = button.__call__("get_parent_window");
console.log(w2);

win.__watch_signal__("destroy");
button.__watch_signal__("clicked");

win.on("destroy", function() {
    console.log("destroyed", arguments[0] instanceof gtk.Window);
    gtk.mainQuit();
});
button.on("clicked", function() {
    console.log("click :)", arguments[0] instanceof gtk.Button, arguments[0] == button);
});

console.log(win.__call__("set_property", "name", "test"));
console.log(win.__get_property__("name"));


gtk.main();
