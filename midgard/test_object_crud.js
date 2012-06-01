
var vows   = require('vows'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

var GObject = require('./gobject');

var Midgard, gir, config, mgd;
Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');

var suite = vows.describe('Midgard.Object');
suite.addBatch({
    'CRUD' : {
        'Create' : {
            topic : function () {
                return MidgardTest.TestBook.create()
            },
            'should be true' : function (topic) {
                assert.isTrue (topic)
            }	
        }
    }
}).run();
