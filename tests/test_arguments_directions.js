
var mocha = require('mocha'),
    should = require('should');

var TestGIR = require('./TestGIR');
var GdkPixbuf = TestGIR.GdkPixbuf;
var GObject = TestGIR.GObject;
var win = TestGIR.win;

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
            win.set_property("opacity", 0.5)
        });

        it('null', function() {
            win.set_icon(null);
        });

        it('object', function() {
            var pixbuf = new GdkPixbuf.Pixbuf(0, false, 1, 1, 1);
            win.set_icon(pixbuf);
        });
    });

    describe('out', function() {

        /* FIXME: Fails with "Error: IN arguments conversion failed" */
        function pendingGetPropertyTest() {
            win.set_title("Lancelot");
            var title = null;
            win.get_property("title", title);
            title.should.equal("Lancelot");
        };
        it('get_property')

	    it('integer', function() {
            var type = GObject.type_from_name("GtkWindow");
            var children = GObject.type_children(type); // hidden, implicit array length
            //children.should.not.equal(null);
            children.length.should.not.be.below(1);
            children.should.include(GObject.type_from_name('GtkDialog'));
            children.should.include(GObject.type_from_name('GtkAssistant'));
            children.should.include(GObject.type_from_name('GtkPlug'));
            children.should.include(GObject.type_from_name('GtkOffscreenWindow'));
		});
    });

    describe('in out', function() {
        
        /* FIXME: not implemented yet */
        function pendingInOutIntegerTest () {
            var entry = gtk.Entry()
            var insert_position = 0
            var new_insert_position = entry.insert_text("abc123", -1, insert_position);
            new_insert_position.should.be(6)
        };
        it('integer')

    });

});

