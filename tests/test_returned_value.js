
var mocha = require('mocha'),
    should = require('should');

var TestGIR = require('./TestGIR');
var win = TestGIR.win;
var GdkPixbuf = TestGIR.GdkPixbuf;
var GObject = TestGIR.GObject;

var pixbuf = new GdkPixbuf.Pixbuf(0, false, 1, 1, 1);

describe('Return value', function() {

    it('number', function() {
        var int_value = GObject.type_from_name("GtkWindow");
        int_value.should.be.a('number'); 
    });

    it('integer', function() {

    });

    it('char', function() {
        win.title = 'Lancelot';
        win.title.should.be.a('string');
    });

    it('double', function() {

    });

    it('boolean', function() {
        win.get_decorated().should.be.a('boolean');
    });

    it('GValue', function() {

    });

    it('null', function() {
        win.set_icon(null);
        should.strictEqual(null, win.get_icon());
    });

    it('object', function() {
        win.icon = pixbuf;
        win.icon.should.be.a('object');
    });

    it('void', function() {
        var void_value = win.resize(10, 10);
        should.strictEqual(undefined, void_value);
    });

    describe('array', function() {
        it('GType', function() {
            var type = GObject.type_from_name("GtkWindow");
            var gtype_array = GObject.type_children(type);
            gtype_array.length.should.not.equal(0);
        });
    });
});

