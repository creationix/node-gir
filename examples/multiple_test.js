var gir = require('../gir')
 ,  gtk = gir.import('Gtk', '3.0')
 ,  WebKit = gir.import('WebKit', '3.0')
 ,  notify = gir.import('Notify')
 ,  appTitle = 'WebKit with Notify Example';

gtk.init(0);
notify.init(appTitle);

var win = new gtk.Window();

win.onDestroy(function() {
  console.log('Window destroyed');
  gtk.mainQuit();
  process.exit();
});

var sw = new gtk.ScrolledWindow();
win.add(sw);

var view = new WebKit.WebView();

view.onTitleChanged(function() {
  win.title = view.title + ' - ' + appTitle;
});

view.onLoadFinished(function() {
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

gtk.main();
