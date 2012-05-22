
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
	'Method' : {
		'set_modal' : {
			topic: win, 
			'should be true': function (topic) {
				topic.set_modal(true);
				assert.isTrue(topic.get_modal());
			},
			'should be false': function (topic) {
				topic.set_modal(false);
				assert.isFalse(topic.get_modal());
			}
		},
		'set_title' : {
			topic: win, 
			'Lancelot': function (topic) {
				topic.set_title("Lancelot");
				assert.equal(topic.get_title(), "Lancelot");
			},
			'Lancelot': function (topic) {
				// Not sure if it's intentional in Gtk.Window API
				// We expect the title we set for exported objects.
				assert.equal(topic.get_title(), "Node.JS GTK Window");
				assert.notEqual(topic.get_title(), "");
				assert.notEqual(topic.get_title(), " ");
				assert.notEqual(topic.get_title(), null);
			}
		},
		'set_opacity' : {
			topic: win, 
			'set double 12,34': function (topic) {
				topic.set_opacity(12.34);
				assert.equal(topic.get_opacity(), 12.34);
			},
			'get double 12,34': function (topic) {
				assert.equal(topic.get_opacity(), 12.34);
				assert.notEqual(topic.get_opacity(), 0,001);
				assert.notEqual(topic.get_opacity(), 2);
				assert.notEqual(topic.get_opacity(), 1,23);
			}
		},
		'set_icon' : {
			topic: win, 
			'set object': function (topic) {
				pixbuf = new gdkPixbuf.Pixbuf(0, false, 1, 1, 1);
				topic.set_icon(pixbuf);
				assert.equal(topic.get_icon(), pixbuf);
			},
			'get object': function (topic) {
				assert.notEqual(topic.get_icon(), null);
				assert.notEqual(topic.get_icon(), topic);
			}
		}
	}
}).export(module);
