
var mocha = require('mocha'),
    should = require('should');

var gir = require('../../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');
Midgard.init();

config = new Midgard.Config();
config.dbdir = '/tmp';
config.dbtype = 'SQLite';
config.database = 'node_gir';

mgd = new Midgard.Connection();

describe('Midgard.Config', function() {
    
    it('Open config', function() {
        var opened = mgd.open_config(config);
        opened.should.be.a('boolean');
        opened.should.equal(true);
    });
});

