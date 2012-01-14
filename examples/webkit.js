var gir = require('../gir')
 ,  gtk = gir.load('Gtk', '3.0')
 ,  WebKit = gir.load('WebKit', '3.0');
 
gtk.init(0);

var win = new gtk.Window();

win.on('destroy', function() {
  console.log('Window destroyed');
  gtk.mainQuit();
  process.exit();
});

var sw = new gtk.ScrolledWindow();
win.add(sw);

var view = new WebKit.WebView();
view.loadUri("http://www.google.com/");
sw.add(view);

win.setSizeRequest(640, 480);
win.showAll();

win.title = 'Node.JS Webkit Example';

gtk.main();
