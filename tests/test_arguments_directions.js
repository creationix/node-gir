
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

var suite = vows.describe('Arguments direction');
suite.addBatch({
	'Argument' : {
		'in' : {
			topic: win, 
			'integer': function (topic) {
				topic.set_property("default_height", 1);
			},
			'string': function (topic) {
				topic.set_property("title", "Lancelot");
			},
			'boolean': function (topic) {
				topic.set_property("modal", true);
			},
			'double': function (topic) {
                assert.equal("TODO", "DONE");
			},
			'null': function (topic) {
                topic.set_icon(null);
			},
			'object': function (topic) {
                pixbuf = new gdkPixbuf.Pixbuf(0, false, 1, 1, 1);
                topic.set_icon(pixbuf);
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
                var type = gobject.type_from_name("GtkWindow");
                var children = gobject.type_children(type); // hidden, implicit array length
                assert.isNotNull(children);
				assert.notEqual(children.length, 0);
                assert.include(children, gobject.type_from_name("GtkDialog"));
                assert.include(children, gobject.type_from_name("GtkApplicationWindow"));
                assert.include(children, gobject.type_from_name("GtkAssistant"));
                assert.include(children, gobject.type_from_name("GtkPlug"));
                assert.include(children, gobject.type_from_name("GtkOffscreenWindow"));
			}
		},
		'in out' : {
			topic: win, 
			'integer': function (topic) {
                assert.equal("TODO", "DONE");
			}
		}
	}
}).export(module);
