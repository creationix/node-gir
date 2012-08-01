
var mocha = require('mocha'),
    should = require('should');

var gir = require('../gir');
gir.init();

var GLib = gir.load('GLib');
var GObject = gir.load('GObject');

var gtk = gir.load('Gtk', '3.0');
gtk.init(0);

describe('Object constructor', function() {

    describe('with properties', function() {

        it('derived from interface', function() {
            var box = new gtk.Box( { orientation : gtk.Orientation.vertical } );
            box.orientation.should.equal(gtk.Orientation.vertical); 
        });
    });
});

