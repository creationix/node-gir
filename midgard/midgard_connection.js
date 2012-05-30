
var vows   = require('vows'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

var Midgard, gir, config, mgd;
Midgard = gir.load('Midgard');
Midgard.init();

config = new Midgard.Config();
config.__set_property__("dbdir", "/tmp");
config.__set_property__("dbtype", "SQLite");
config.__set_property__("database", "node_gir");

mgd = new Midgard.Connection();
mgd.open_config(config);

exports.cnc = mgd;
