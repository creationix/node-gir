var gir = require('../gir')
 ,  clutter = gir.load('Clutter');
 
clutter.init(0);

//I used .prototype because I need a static object. Not sure if this is right.
var sm = clutter.StageManager.prototype;

console.log('StageManager:', sm);

/* TODO: Fix throws TypeError: Illegal invocation */
var stage = sm.getDefaultStage();

//for (var s in clutter) console.log(s);

console.log('StageManager:', sm, 'Stage:', stage);

stage.title = 'Node.JS Clutter Example';
stage.setSize(400,300);
stage.setColor(0,0,0,127);
stage.show();

stage.on('button-press-event', function(a,b,c) {
  console.log('button press event', a, b, c);
});

stage.on('destroy', function() {
  console.log('destroy');
  clutter.mainQuit();
  process.exit();
});

clutter.main();
