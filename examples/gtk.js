var gir = require('../gir')
 ,  gtk = module.exports = gir.load('Gtk', '3.0');
 
gtk.init(0);

var win = new gtk.Window({type: gtk.WindowType.toplevel, title:"Node.JS GTK Window"});
var button = new gtk.Button();

win.borderWidth = 10;

button.label = "CLICK ME!";

win.add(button);
win.showAll();

var w2 = button.getParentWindow();
console.log(w2);

win.on("destroy", function() {
    console.log("destroyed", arguments[0] instanceof gtk.Window);
    gtk.mainQuit();
});
button.on("clicked", function() {
    console.log("click :)", arguments[0] instanceof gtk.Button, arguments[0] == button);
});

console.log(win.name = "test");
console.log(win.name);

gtk.main();
