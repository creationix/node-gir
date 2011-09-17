var gtk = require("./gtk"),
    WebKit = require("./webkit");

gtk.init(0);

var win = new gtk.Window();

var sw = new gtk.ScrolledWindow();
win.add(sw);

var view = new WebKit.WebView();
view.loadUri("http://www.google.com/");
sw.add(view);

win.setSizeRequest(640, 480);
win.showAll();

gtk.main();

