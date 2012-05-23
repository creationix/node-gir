
var gir = require('gir');
gir.init();

//Let's use gtk window for testing purpose
var gtk = require("./gtk");
gtk.init(0);

exports.win = new gtk.Window({type: gtk.WindowType.toplevel, title:"Node.JS GTK Window"});
