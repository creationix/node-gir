
var vows   = require('vows'),
    assert = require('assert');

var gir = require('gir');
gir.init();

var Midgard, gir, config, mgd;
Midgard = gir.load('Midgard');
Midgard.init();

config = new Midgard.Config();
config.__set_property__("dbdir", "/tmp");
config.__set_property__("dbtype", "SQLite");
config.__set_property__("database", "node_gir");

mgd = new Midgard.Connection();

var suite = vows.describe('Midgard.Config');
suite.addBatch({
	'Open Config' : {
		topic: mgd.__call__('open_config', config),
		'should be true': function (topic) {
			assert.isTrue(topic);
		}
	}
}).run();
