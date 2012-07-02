
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

var suite = vows.describe('Gtk.Object');
suite.addBatch({
	'Property' : {
		'boolean' : {
			topic: win, 
			'should be true': function (topic) {
				topic.modal = true;
				assert.isTrue(topic.modal);
			},
			'should be false': function (topic) {
				topic.modal = false;
				assert.isFalse(topic.modal);
			}
		},
		'string' : {
			topic: win, 
			'set Lancelot': function (topic) {
				topic.title = "Lancelot";
				assert.equal(topic.title, "Lancelot");
			},
			'get Lancelot': function (topic) {
				assert.equal(topic.title, "Lancelot");
				assert.notEqual(topic.title, "");
				assert.notEqual(topic.title, " ");
				assert.notEqual(topic.title, null);
			}
		},
		'integer' : {
			topic: win, 
			'set 1': function (topic) {
				topic['default-height'] = 1;
				assert.equal(topic['default-height'], 1);
			},
			'get 1': function (topic) {
				assert.equal(topic['default-height'], 1);
				assert.notEqual(topic['default-height'], 0);
				assert.notEqual(topic['default-height'], 2);
				assert.notEqual(topic['default-height'], -1);
			}
		},
		'double' : {
			topic: win, 
			'set 12,34': function (topic) {
				topic.opacity = 12.34;
				assert.equal(topic.opacity, 12.34);
			},
			'get 12,34': function (topic) {
				assert.equal(topic.opacity, 12.34);
				assert.notEqual(topic.opacity, 0,001);
				assert.notEqual(topic.opacity, 2);
				assert.notEqual(topic.opacity, 1,23);
			}
		},
		'object' : {
			topic: win, 
			'set object': function (topic) {
				pixbuf = new gdkPixbuf.Pixbuf(0, false, 1, 1, 1);
				topic.icon = pixbuf;
				assert.equal(topic.icon, pixbuf);
			},
			'get object': function (topic) {
				assert.notEqual(topic.icon, null);
				assert.notEqual(topic.icon, topic);
			}
		}
	}
}).run();
