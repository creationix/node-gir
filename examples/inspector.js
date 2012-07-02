var gir = require('../gir')
 ,  util = require('util');

console.warn('[inspector] Using node version', process.version + '.');

process.argv.forEach(function (val, index, args) {
  if (args.length < 3) {
    console.log('Usage: node inspector [options] type [version [object [object ...]]]');
    console.log('Description: Inspects each type name passed as an argument.');
    console.log('');
    console.log('Syntax 1: node inspector type [version [object [object ...]]]');
    console.log('Syntax 2: node inspector [-v] type [version]');
    console.log('Syntax 3: node inspector [-g] type [version] [object]');
    console.log('');
    console.log('Parameters:');
    console.log(' options: optional, one or more options.');
    console.log(' type: required type name to load from girepository.');
    console.log(' version: optional, type version to load from girepository.');
    console.log(' object: optional, multiple, fields you intend to inspect.');
    console.log('');
    console.log('Options:');
    console.log('');
    console.log(' -v Prints version list for a given type. Do not provide objects with -v.');
    console.log(' -g Loads in GTK GUI mode. Only the first object will be browsed, if provided.');
    console.log('');
    console.log('Example 1: node inspector Gtk');
    console.log(' Prints tree for Gtk to stdout.');
    console.log('');
    console.log('Example 2: node inspector Gtk 3.0');
    console.log(' Prints tree for Gtk version 3.0 to stdout.');
    console.log('');
    console.log('Example 3: node inspector Notify > notify_tree.txt');
    console.log(' Prints tree for Notify with stdout redirected to a file.');
    console.log('');
    console.log('Example 4: node inspector Clutter 1.0 Actor Texture');
    console.log(' Prints tree for Clutter version 1.0 Actor and Texture objects.');
    console.log('');
    console.log('Example 5: node inspector -g Gtk 3.0 Window');
    console.log(' Opens GUI and browses to the GTK 3.0 Window object.');
    console.log('');
    process.exit();
  }
  else {
    if (args[2] == '-v') {
      dumpTypeVersions(importType(args[3]));
      process.exit();
    }
    else if (args[2] == '-g') {
      if (args.length > 3)
        launchGtk.apply(this, args.slice(3));
      else
        launchGtk();
      process.exit();
    }
    var mod = args[2], ver = args[3];
    var module = importType(mod, ver);
    if (ver != undefined) {
      if (args.length > 4) for (var a = 4; a < args.length; a++) {
        dumpTypeInfo(module, args[a]);
      }
      else
        dumpTypeInfo(module);
    }
    else
        dumpTypeInfo(module);
    process.exit();
  }
});

/**
 * Imports a girepository type with a specified version.
 **/
function importType(type, ver) {
  if (ver != undefined)
    try {
      return gir.import(type, ver);
    }
    catch (e) {
      console.warn('In importType(),', e);
      return gir.import(type);
    }
  else
    return gir.import(type);
}

/**
 * Prints a type or some of its objects out to a string.
 **/
function getTypeInfo(loadedType, obj) {
  if (obj != undefined)
    return  'Type information for .import(\'' + loadedType.__name__ + '\',\'' + loadedType.__version__ + '\').' + obj + ':' +
            '\n' + util.inspect(loadedType[obj]) + '\n';
  else
    return  'Type information for .import(\'' + loadedType.__name__ + '\',\'' + loadedType.__version__ + '\')' + ':' +
            '\n' + util.inspect(loadedType) + '\n';
}

/**
 * Prints a type or some of its objects out to a string.
 **/
function getTypeVersions(loadedType) {
  return gir._girepository.getVersions(loadedType.__name__);
}

/**
 * Dumps a type or some of its objects out to stdout.
 **/
function dumpTypeInfo(loadedType, obj) {
  if (obj != undefined)
    console.log(getTypeInfo(loadedType, obj));
  else
    console.log(getTypeInfo(loadedType));
}

/**
 * Dumps type versions to stdout.
 **/
function dumpTypeVersions(loadedType) {
  console.log("'"+loadedType.__name__+"'", 'available in versions', getTypeVersions(loadedType));
}

/**
 * In progress: Dumps complete tree of every member of every installed version
 * of every object returned when loading the namespace from girepository.
 * Will appear in the tree view.
 **/
function gtkDumpTypeTree(win, type, brwsVer, brwsObj) {
console.log(win, type, brwsVer, brwsObj);
  var loadedType = importType(type, brwsVer);
  //dump info to gtk
  gtkDumpTypeInfo(win, loadedType, brwsObj);
  //dump info to console
  dumpTypeInfo(loadedType, brwsObj);
}

/**
 * Dumps type information to GTK window.
 **/
function gtkDumpTypeInfo(win, loadedType, obj) {
  if (obj != undefined)
    win.textInfoBuffer.text = getTypeInfo(loadedType, obj);
  else
    win.textInfoBuffer.text = getTypeInfo(loadedType);
}

/**
 * Launches the GTK GUI. Returns the Gtk.Window object.
 **/
function launchGtk(type, brwsVer, brwsObj) {
  var gtk = gir.import('Gtk');
  console.warn('[inspector] Using GTK version', gtk.__version__ + '.');
  gtk.init(0);
  
  var win = new gtk.Window({type: gtk.WindowType.toplevel, title:'GIR Inspector', name:'GIR Inspector'});
  win.gtk = gtk;
  win.vboxOuter = new win.gtk.VBox(1,2);
  win.hboxInner = new win.gtk.HBox(1,2);
   
  win.menuBar = new win.gtk.MenuBar();
   
  win.menuFile = new win.gtk.Menu();
  win.menuFileFile = new win.gtk.MenuItem('File');
  win.menuFileQuit = new win.gtk.MenuItem('Quit');
   
  win.menuSamples = new win.gtk.Menu();
  win.menuSamplesSamples = new win.gtk.MenuItem('Samples');
   
  win.menuHelp = new win.gtk.Menu();
  win.menuHelpHelp = new win.gtk.MenuItem('Help');
  win.menuHelpAbout = new win.gtk.MenuItem('About...');
 
  //these menus do not work, and i greatly suspect it may have something to do with the issues below
  win.menuFileFile.setSubmenu(win.menuFile);
  win.menuSamplesSamples.setSubmenu(win.menuSamples);
  win.menuHelpHelp.setSubmenu(win.menuHelp);
  
  win.menuFile.append(win.menuFileQuit);
  win.menuHelp.append(win.menuHelpAbout);

  win.menuBar.append(win.menuFile);
  win.menuBar.append(win.menuSamples);
  win.menuBar.append(win.menuHelp);
  
  win.vboxOuter.packStart(win.menuBar, false, true, 0);
  win.vboxOuter.packStart(win.hboxInner, true, true, 0);

  win.vboxInnerLeft = new win.gtk.VBox(1,2);
  win.hboxInnerLeftTop = new win.gtk.HBox(1,3);

  win.treeStore = new win.gtk.TreeStore('Inspect');
  win.treeView = new win.gtk.TreeView();
  win.treeView.model = win.treeStore;
  win.treeColumn = new win.gtk.TreeViewColumn({title:'Inspect'});
  win.treeView.appendColumn(win.treeColumn);
  win.treeView.heightRequest = 300;
 
  win.labelSearch = new win.gtk.Label({label: 'Object Type:'});
  win.entrySearch = new win.gtk.Entry();
  win.entrySearch.widthRequest = 200;
  win.buttonSearch = new win.gtk.Button({label: 'Import Object'});
  win.buttonSearch.onClicked(function () {
    gtkDumpTypeTree(win, win.entrySearch.text)
  });
   
  win.scrolledTextWindow = new win.gtk.ScrolledWindow();
  win.textInfo = new win.gtk.TextView();
  win.textInfoBuffer = win.textInfo.getBuffer();
  win.textInfoBufferFontTag = new win.gtk.TextTag();
  //win.textInfoBufferFontTag = win.textInfoBuffer.createTag('monofont'); //fails, see next comment
  win.textInfoBufferFontTag.family = 'monospace';
  win.textInfoBuffer.tagTable.add(win.textInfoBuffer);
//debug:  for (var x in win.gtk.TextBuffer.__methods__) console.log(win.gtk.TextBuffer.__methods__[x]);
  win.textInfoBuffer.text = 'Enter an object type name in the search box, then click Import Object.';
// this is broken! i think the problem is in object.cc in CallMethod(). basically, CallMethod()
// is returning a valid object with __properties__ and friends, but for some reason it left it's
// good old pals __call__, __get_property__, etc. behind. that means the object returned by a
// method can not be further interacted with. i can't seem to find the the perfect spot to plant
// an additional call to GIRObject::SetPrototypeMethods() in or around CallMethod() or the function.cc
// Func::Call(). it returns 'bad arguments', but it is the complete lack of __call__ on the object
// causing the error. --david
//  win.textInfoBuffer.applyTag(win.textInfoBufferFontTag, win.textInfoBuffer.getStartIter(), win.textInfoBuffer.getEndIter());
  win.textInfo.editable = false;
  win.scrolledTextWindow.widthRequest = 480;

  win.hboxInnerLeftTop.packStart(win.labelSearch, false, false, 5);
  win.hboxInnerLeftTop.packStart(win.entrySearch, false, false, 5);
  win.hboxInnerLeftTop.packStart(win.buttonSearch, false, false, 0);
  win.vboxInnerLeft.packStart(win.hboxInnerLeftTop, false, false, 5);
  win.vboxInnerLeft.packStart(win.treeView, true, true, 0);

  win.hboxInner.packStart(win.vboxInnerLeft, false, true, 5);
  win.scrolledTextWindow.add(win.textInfo);
  win.hboxInner.packStart(win.scrolledTextWindow, true, true, 0);
  
  win.add(win.vboxOuter);
  
  win.onDestroy(function() {
    win.gtk.mainQuit();
    process.exit();
  });
  win.menuFileQuit.onActivate(function () { win.destroy(); });
  
  win.menuBar.showAll();
  win.showAll();
  
  if (type != undefined) {
    win.entrySearch.text = type;
    gtkDumpTypeTree(win, type, brwsVer, brwsObj);
  }

  win.heightRequest = 500;
  win.gtk.main();
  
  return win;
}
