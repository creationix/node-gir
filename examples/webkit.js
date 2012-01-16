var gir = require('../gir')
 ,  gtk = gir.import('Gtk', '3.0')
 ,  WebKit = gir.import('WebKit', '3.0')
 ,  appTitle = 'Webkit Example';
 
gtk.init(0);

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

view.loadUri("http://www.google.com/");
sw.add(view);

win.setSizeRequest(640, 480);
win.showAll();

win.title = appTitle;

gtk.main();
