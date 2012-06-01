
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
            'is created' : function (topic) {
                assert.isTrue(topic);
            },	
            'is valid guid' : function (topic) {
                assert.isTrue(Midgard.is_guid(MidgardTest.TestBook.guid));
            }	
        },
        'Update' : {
            topic : function () {
                MidgardTest.TestBook.name = "The Holy Grail 2";
                return MidgardTest.TestBook.update()
            },
            'is updated' : function (topic) {
                assert.isTrue(topic);
            },	
            'is valid guid' : function (topic) {
                assert.isTrue(Midgard.is_guid(MidgardTest.TestBook.guid));
            }	
        },
        'Delete' : {
            topic : function () { 
                return MidgardTest.TestBook.delete(false)
            },
            'is deleted' : function (topic) {
                assert.isTrue(topic);
            }
        },
        'Purge' : {
            topic : function () {
                MidgardTest.TestBook.create();
                return MidgardTest.TestBook.purge(false)
            },
            'is purged' : function (topic) {
                assert.isTrue(topic);
            }	
        }
    }
}).run();
