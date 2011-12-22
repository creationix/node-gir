var gir = require("../gir");
gir.init();

var gtk = require("./gtk");
var WebKit = require("./webkit");

gtk.init(0);

var win = new gtk.Window();

win.__watch_signal__("destroy")
win.on('destroy', function() {
  console.log('Window destroyed');
  gtk.mainQuit();
  process.exit();
});

var sw = new gtk.ScrolledWindow();
win.__call__("add", sw);

var view = new WebKit.WebView();
view.__call__("load_uri", "http://www.google.com/");
sw.__call__("add", view);

win.__call__("set_size_request", 640, 480);
win.__call__("show_all");

gtk.main();

