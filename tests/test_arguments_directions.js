
var mocha = require('mocha'),
    should = require('should');

var gir = require('../gir');
gir.init();

var GLib = gir.load('GLib');
var GObject = gir.load('GObject');

// Let's use gtk window for testing purpose
var gtk = require("./gtk");
gtk.init(0);

var gdkPixbuf = require("./gdkPixbuf");

var objects = require('./objects');
var win = objects.win;

describe('Arguments direction', function() {

    describe('in', function() {

        it('integer', function() {
            win.set_property("default_height", 1);
        });

        it('string', function() {
            win.set_property("title", "Lancelot");
        });

        it('boolean', function() {
            win.set_property("modal", true);
        });

        it('double', function() {
            var done = "TODO";
            done.should.equal("DONE");
        });

        it('null', function() {
            win.set_icon(null);
        });

        it('object', function() {
            var pixbuf = new gdkPixbuf.Pixbuf(0, false, 1, 1, 1);
            win.set_icon(pixbuf);
        });
    });

    describe('out', function() {

        it('get_property', function() {
            win.set_title("Lancelot");
            var title = null;
            win.get_property("title", title);
            title.should.equal("Lancelot");
        });
          
	    it('integer', function() {
            var type = GObject.type_from_name("GtkWindow");
            var children = GObject.type_children(type); // hidden, implicit array length
            //children.should.not.equal(null);
            children.length.should.not.be.below(1);
            children.should.include(GObject.type_from_name('GtkDialog'));
            children.should.include(GObject.type_from_name('GtkApplicationWindow'));
            children.should.include(GObject.type_from_name('GtkAssistant'));
            children.should.include(GObject.type_from_name('GtkPlug'));
            children.should.include(GObject.type_from_name('GtkOffscreenWindow'));
		});
    });

    describe('in out', function() {
        
        it('integer', function() {
            var done = "TODO";
            done.should.equal("DONE");
        });
    });
});

