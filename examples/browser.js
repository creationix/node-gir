var gtk = require("./gtk")
 , WebKit = require("./webkit");

gtk.init(0);

var win = new gtk.Window();

win.on('destroy', function() {
  console.log('Window destroyed');
  gtk.main_quit();
  process.exit();
});

var sw = new gtk.ScrolledWindow();
win.add(sw);

var view = new WebKit.WebView();
view.load_uri("http://www.google.com/");
sw.add(view);

win.set_size_request(640, 480);
win.show_all();

gtk.main();
