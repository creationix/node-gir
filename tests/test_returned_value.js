
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
                var int_value = gobject.type_from_name("GtkWindow");
                assert.isNumber(int_value);
            }
        },
        'char' : {
            topic: win, 
            'char': function (topic) {
                topic.set_title("Lancelot");
                var char_value = topic.get_title();
                assert.isString(char_value);
            }
        },
        'double' : {
            topic: win, 
            'double': function (topic) {
                assert.equal("TODO", "DONE");
            }
        },
        'boolean' : {
            topic: win, 
            'bool': function (topic) {
                var boolean_value = topic.is_active();
                assert.isBoolean(boolean_value);
            }
        },
        'GValue' : {
            topic: win, 
            'GValue': function (topic) {
                assert.equal("TODO", "DONE");
            }
        },
        'null' : {
            topic: win, 
            'null': function (topic) {
                var null_value = topic.get_icon();
                assert.isNull(null_value);
            }
        },
        'object' : {
            topic: win, 
            'object': function (topic) {
                pixbuf = new gdkPixbuf.Pixbuf(0, false, 1, 1, 1);
                topic.set_icon(pixbuf);
                var object_value = topic.get_icon();
                assert.isObject(object_value);
            }
        },
        'void' : {
            topic: win, 
            'void': function (topic) {
                var void_value = topic.set_icon(null);
                assert.isUndefined(void_value);
            }
        },
        'array' : {
            topic: win, 
            'GType': function (topic) {
                var type = gobject.type_from_name("GtkWindow");
                var gtype_array = gobject.type_children(type);
                assert.isArray(gtype_array);
            },
            'Object': function (topic) {
                assert.equal("TODO", "DONE");
            }
        }
    }
}).export(module);
