var gir = require("../gir"),
    gtk = gir.load("Gtk"),
    WebKit = gir.load("WebKit");

gtk.init(0);

var win = new gtk.Window({
    name: "main_window",
    title: "test"
});

var sw = new gtk.ScrolledWindow();
win.add(sw);

var view = new WebKit.WebView();
view.loadUri("http://www.google.com/");
sw.add(view);

win.setSizeRequest(640, 480);
win.showAll();

win.title = "test2";

// change the ee that we automatically call __watch_signal__
// also make sure that we call unwatch or sth like that when there is no cb left
sw.__watch_signal__("destroy");
sw.on("destroy", function() {
    console.log("destroy");
    gtk.mainQuit();
});

gtk.main();

