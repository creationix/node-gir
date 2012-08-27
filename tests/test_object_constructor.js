
var mocha = require('mocha'),
    should = require('should');

var gir = require('../gir');
gir.init();

var GLib = gir.load('GLib');
var GObject = gir.load('GObject');

var Gtk = gir.load('Gtk', '3.0');
Gtk.init(0);

describe('Object constructor', function() {

    describe('with "new" operator', function() {

        it('without arguments', function() {
            var label = new Gtk.Label();
            label.label.should.equal("");
        });

        it('with properties', function() {
            var label = new Gtk.Label( { label: 'aText99'} );
            label.label.should.equal('aText99');
        });
    });

    describe('using static functions', function() {

        /* FIXME: pending. Calling .new() ends up calling GObject.Object.new() directly and not
         * the .new() of Gtk.Button. Ordering problem in registration of static functions? */
        it('new()', null, function() {
            var button = Gtk.Button.new();
            button.label.should.equal(null);
        });

        it('new_with_argument(arg)', function() {
            var button = Gtk.Button.new_with_label('myLabel312');
            button.label.should.equal('myLabel312');
        });

    });

    describe('with properties', function() {

        it('derived from interface', function() {
            var box = new Gtk.Box( { orientation : Gtk.Orientation.vertical } );
            box.orientation.should.equal(Gtk.Orientation.vertical);
        });
    });

});

