
var mocha = require('mocha'),
    should = require('should');

var gir = require('../../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');
Midgard.init();

var config = new Midgard.Config();
config.read_file_at_path('test_SQLITE.conf');

var mgd = new Midgard.Connection();
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
  
        it('connected', function() {
            mgd.open_config(config);
            from_connection_callback.should.equal('MGD_ERR_OK');
            from_connection_callback = '';
        });

        it('error', function() {
            obj = Midgard.Object.factory(mgd, "gir_test_book_store");
            obj.create();
            from_connection_callback.should.equal('MGD_ERR_OK');
            obj.purge(false);
            from_connection_callback.should.equal('MGD_ERR_OK');
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
    });
});
