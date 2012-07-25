
var mocha = require('mocha'),
    should = require('should');

var gir = require('../../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');
Midgard.init();

var config = new Midgard.Config();
config.read_file_at_path(__dirname + '/test_SQLITE.conf');

var MidgardTest = require('./midgard_connection');

var mgd = MidgardTest.cnc
var from_connection_callback = '';
var from_object_callback = '';
var obj = null;

// Callbacks
mgd.on('error', function() {
    from_connection_callback = mgd.get_error_string();
});

// Tests

describe('Signals', function() {
    
    describe('Midgard.Connection', function() {
  
        it('create object error', function() {
            obj = Midgard.Object.factory(mgd, "gir_test_book_store");
            obj.create();
            from_connection_callback.should.equal('MGD_ERR_OK');
        });

        it('purge object error', function() {
            obj.purge(false);
            from_connection_callback.should.equal('MGD_ERR_OK');
        });

        it('invalid signal name', function() {
            try {
                win.on('invalid-name', function() { });
            } catch (err) {
                // do nothing
            }
        });

    });

    describe('Midgard.Object', function() {
  
        it('action-create', function() {
            obj = Midgard.Object.factory(mgd, "gir_test_book_store");
            obj.on('action-create', function() {
                from_object_callback = 'Object create';
            });
            obj.create();
            from_object_callback.should.equal('Object create');
            from_connection_callback.should.equal('MGD_ERR_OK');
        });

        it('action-purge', function() {
            obj = Midgard.Object.factory(mgd, "gir_test_book_store");
            obj.on('action-purge', function() {
                from_object_callback = 'Object purge';
            });
            obj.purge('false');
            from_object_callback.should.equal('Object purge');
            from_connection_callback.should.equal('MGD_ERR_OK');
        });

        it('invalid signal name', function() {
            try {
                obj.on('invalid-name', function() { });
            } catch (err) {
                // do nothing
            }
            try {
                obj.on('invalid_name', function() { });
            } catch (err) {
                // do nothing
            }
        });
    });
});
