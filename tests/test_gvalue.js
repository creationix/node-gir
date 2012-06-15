
var vows   = require('vows'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

// Let's use gtk window for testing purpose
var gtk = require("./gtk");
gtk.init(0);

var GObject = require("./gobject");
var objects = require('./objects');
var win = objects.win;

var suite = vows.describe('GObject.GValue');
suite.addBatch({
    'Set value' : {
        topic: new GObject.Value(), 
        'string': function (topic) {
            var str = "Test String";
	    //topic.init(16);
            topic.set_string(str);
            assert.isString(topic.get_string());
            assert.equal(topic.get_string(), str);
        }
    }
}).run();
