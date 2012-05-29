
var vows   = require('vows'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

// Let's use gtk window for testing purpose
var gtk = require("./gtk");
gtk.init(0);

var gdkPixbuf = require("./gdkPixbuf");
var glib = require("./glib");
var gobject = require("./gobject");

var objects = require('./objects');
var win = objects.win;

var suite = vows.describe('Returned value');
suite.addBatch({
    'return' : {
        'integer' : {
            topic: win, 
            'integer': function (topic) {
                assert.equal("TODO", "DONE");
            }
        },
        'char' : {
            topic: win, 
            'integer': function (topic) {
                assert.equal("TODO", "DONE");
            }
        },
        'double' : {
            topic: win, 
            'integer': function (topic) {
                assert.equal("TODO", "DONE");
            }
        },
        'boolean' : {
            topic: win, 
            'integer': function (topic) {
                assert.equal("TODO", "DONE");
            }
        },
        'GValue' : {
            topic: win, 
            'integer': function (topic) {
                assert.equal("TODO", "DONE");
            }
        },
        'object' : {
            topic: win, 
            'integer': function (topic) {
                assert.equal("TODO", "DONE");
            }
        },
        'null' : {
            topic: win, 
            'integer': function (topic) {
                assert.equal("TODO", "DONE");
            }
        },
        'void' : {
            topic: win, 
            'integer': function (topic) {
                assert.equal("TODO", "DONE");
            }
        },
    }
}).export(module);
