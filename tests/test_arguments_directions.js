
var vows   = require('vows'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

// Let's use gtk window for testing purpose
var gtk = require("./gtk");
gtk.init(0);

var gdkPixbuf = require("./gdkPixbuf");
var glib = require("./glib");

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

			},
			'integer': function (topic) {
				var txt = "abcd";
				var length;
				var decoded = glib.base64_decode(txt, length);
				assert.notEqual(length, 0);
				assert.equal(length, 4);
			}
		},
		'in out' : {
			topic: win, 
			'integer': function (topic) {
				var txt = "abcd";
				var length;
				var decoded = glib.base64_decode_inplace(txt, length);
				assert.notEqual(length, 0);
				assert.equal(length, 4);
			}
		}
	}
}).export(module);
