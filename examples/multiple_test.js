var gir = require('../gir')
 ,  gtk = gir.load('Gtk', '3.0')
 ,  WebKit = gir.load('WebKit', '3.0')
 ,  notify = gir.load('Notify')
 ,  appTitle = 'Node.JS Gtk+Webkit+Notify Multiple Example';

gtk.init(0);
notify.init(appTitle);

var win = new gtk.Window();

win.on('destroy', function() {
  console.log('Window destroyed');
  gtk.mainQuit();
  process.exit();
});

var sw = new gtk.ScrolledWindow();
win.add(sw);

var view = new WebKit.WebView();

view.on('title-changed', function() {
  win.title = view.title + ' - ' + appTitle;
});

view.on('load-finished', function() {
  var n = new notify.Notification();
  n.update('URL Loaded', view.uri);
  setTimeout(function () {
    n.close();
  }, 1500);
  n.show();
});

view.loadUri("http://www.yahoo.com/");
sw.add(view);

win.setSizeRequest(640, 480);
win.showAll();

win.title = appTitle;

console.log(WebKit.WebView.__methods__);

gtk.main();
