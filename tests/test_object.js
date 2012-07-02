
var vows   = require('vows'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

// Let's use gtk window for testing purpose
var gtk = require("./gtk");
gtk.init(0);

var objects = require('./objects');
var win = objects.win;

var suite = vows.describe('Gtk.Object');
suite.addBatch({
	'Set Property' : {
		topic: win, 
		'should be true': function (topic) {
			topic.modal = true;
			assert.isTrue(topic.modal);
		},
		'should be false': function (topic) {
			topic.modal = false;
			assert.isFalse(topic.modal);
		}
	}
}).run();
