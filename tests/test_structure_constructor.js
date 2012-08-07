
var mocha = require('mocha'),
    should = require('should');

var gir = require('../gir');
gir.init();

var GLib = gir.load('GLib');
var GObject = gir.load('GObject');

var gtk = gir.load('Gtk', '3.0');
gtk.init(0);

describe('Structure constructor', function() {

    describe('with properties', function() {

        it('GLib.MainLoop', function() {
            var loop = new GLib.MainLoop(null, false);
            loop.should.be.a('object');
            var running = loop.is_running();
            running.should.equal(false);
        });
    });

    describe('without properties', function() {

        it('GLib.MainContext', function() {
            var context = new GLib.MainContext();
            context.should.be.a('object');
        });
    });
});

