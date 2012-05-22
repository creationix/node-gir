
var vows   = require('vows'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

// Let's use gtk window for testing purpose
var gtk = require("./gtk");
gtk.init(0);

var gdkPixbuf = require("./gdkPixbuf");

var objects = require('./objects');
var win = objects.win;

var suite = vows.describe('Arguments direction');
suite.addBatch({
	'Argument' : {
		'in' : {
			topic: win, 
			'set_property': function (topic) {
				topic.set_property("modal", true);
				assert.isTrue(topic.get_modal());
			}
		},
		'out' : {
			topic: win, 
			'get_property': function (topic) {
				topic.set_title("Lancelot");
				var title = null;
				topic.get_property("title", title);
				assert.equal(title, "Lancelot");
				assert.notEqual(title, "");
				assert.notEqual(title, null);

			}
		},
		'in out' : {
			topic: win, 
			'todo': function (topic) {	
				assert.equal("TODO", "DONE");
			}
		}
	}
}).export(module);
