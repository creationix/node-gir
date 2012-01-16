var gir = require('../gir')
 ,  gtk = gir.import('Gtk', '3.0');
 
gtk.init(0);

var win = new gtk.Window({type: gtk.WindowType.toplevel, title:"GTK Example"})
 , vbox = new gtk.VBox(1,3)
 , label = new gtk.Label()
 , button = new gtk.Button()
 , quitButton = new gtk.Button();

win.borderWidth = 10;
win.widthRequest = 200;

button.label = "CLICK ME!";
quitButton.label = "Quit";

vbox.add(button);
vbox.add(label);
vbox.add(quitButton);

win.name = "gtktest";
win.add(vbox);
win.showAll();

//window destroyed
win.onDestroy(function() {
  //debug:console.log("destroyed", arguments[0] instanceof gtk.Window);
  gtk.mainQuit();
  process.exit();
});

var clicks = 0;
//user clicked CLICK ME! button
button.onClicked(function() {
  //debug:console.log("click :)", arguments[0] instanceof gtk.Button, arguments[0] == button);
  //testing objects returned from members in c-land:console.log("click :)", button, button, button.window);
  label.label = 'Clicked ' + ++clicks + ' times.';
});

//user clicked Quit button
quitButton.onClicked(function() {
  win.destroy();
});

//start event loop
gtk.main();
