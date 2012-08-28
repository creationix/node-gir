
var mocha = require('mocha'),
    should = require('should');

var TestGIR = require('./TestGIR');
var win = TestGIR.win;
var GdkPixbuf = TestGIR.GdkPixbuf;

var pixbuf = new GdkPixbuf.Pixbuf(0, false, 1, 1, 1);

describe('Gtk.Object', function() {

    describe('property', function() {
        
        describe('boolean', function() {

            it('should be true', function() {
                win.modal = true;
                win.modal.should.equal(true);
            });
            
            it('should be false', function() {
                win.modal = false;
                win.modal.should.equal(false);
            });
        });

        describe('string', function() {

            it('set Lancelot', function() {
                win.title = 'Lancelot';
                win.title.should.equal('Lancelot');
            });
            
            it('get Lancelot', function() {
                win.title.should.equal('Lancelot');
                win.title.should.not.equal('');
                win.title.should.not.equal(' ');
            });
        });

        describe('integer', function() {

            it('set 1', function() {
                win['default-height'] = 1;
                win['default-height'].should.equal(1);
            });
            
            it('get 1', function() {
                win['default-height'].should.equal(1);
                win['default-height'].should.not.equal(0);
                win['default-height'].should.not.equal(-1);
            });
        });

        describe('double', function() {

            it('set 0.33', function() {
                win.opacity = 0.33;
                win.opacity.should.equal(0.33);
            });
            
            it('get 0.33', function() {
                win.opacity.should.equal(0.33);
                win.opacity.should.not.equal(0.001);
                win.opacity.should.not.equal(2);
                win.opacity.should.not.equal(1.23);
            });
        });

        describe('object', function() {

            it('set icon', function() { 
                win.icon = pixbuf;
            });
            
            it('get icon', function() {
                win.icon.should.be.a('object');
                win.icon.should.not.equal(win);
            });
        });
    });
});
