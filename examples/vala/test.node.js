/* To run this example just type "make run".
 * This Example is forked from https://github.com/antono/vala-object
 */

var gir = require('../../gir');
var ValaObject = gir.load('ValaObject');

ValaObject.say_hello_to('Node.js');

var inst = new ValaObject.ValaClass(); // FIXME Constructor dosn't work
console.log(inst); // => {}
console.log(inst.append_to_name('called from Node.js'));
