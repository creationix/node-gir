
var vows   = require('vows'),
    assert = require('assert');

var TestGIR = require('./TestGIR');
var win = TestGIR.win;

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
