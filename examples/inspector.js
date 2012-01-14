var gir = require('../gir');
process.argv.forEach(function (val, index, args) {
  if (args.length < 3) {
    console.log('Usage: node inspector type [version [object [object ...]]]');
    console.log('Description: Inspects each type name passed as an argument.');
    console.log('');
    console.log('Parameters:');
    console.log(' type: required type name to load from girepository');
    console.log(' version: optional, type version to load from girepository');
    console.log(' object: optional, multiple, fields you intend to inspect');
    console.log('');
    console.log('Example 1: node inspector Gtk');
    console.log('  Prints tree for Gtk');
    console.log('');
    console.log('Example 2: node inspector Gtk 3.0');
    console.log('  Prints tree for Gtk version 3.0.');
    console.log('');
    console.log('Example 3: node inspector Notify > notify_tree.txt');
    console.log('  Prints tree for Notify (stdout) to a file.');
    console.log('');
    console.log('Example 4: node inspector Clutter 1.0 Actor Texture');
    console.log('  Prints tree for Clutter version 1.0 Actor and Texture objects.');
    console.log('');
    process.exit();
  }
  else {
    var mod = args[2], ver = args[3];
    if (ver != undefined) {
      var module = gir.load(mod, ver);
      if (args.length > 4) for (var a = 4; a < args.length; a++) {
        console.log('Type information for ' + mod + ' ' + ver + ' / ' + args[a] + ':');
        console.log(module[args[a]])
        console.log('');
      }
      else {
        console.log('Type information for ' + mod + ' ' + ver + ':');
        console.log(module);
        console.log('');
      }
    }
    else {
      var module = gir.load(mod);
      console.log('Type information for ' + mod + ':');
      console.log(module);
      console.log('');
    }
    process.exit();
  }
});
