
var vows   = require('vows'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

var GObject = require('./gobject');

var Midgard, gir, config, mgd;
Midgard = gir.load('Midgard');
Midgard.init();

var mgd = require('./midgard_connection');

var suite = vows.describe('Midgard.Object');
suite.addBatch({
    'CRUD' : {
        'Create' : {
            topic : function () {
                var sdir = Midgard.Object.factory(mgd.cnc, "midgard_snippetdir");
                sdir.name = "Created with node-gir";
                return sdir.create()
            },
            'should be true' : function (topic) {
                assert.isTrue (topic)
            }	
        }
    }
}).run();
