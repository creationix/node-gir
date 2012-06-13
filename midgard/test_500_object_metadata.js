
var vows   = require('vows'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

var GObject = require('./gobject');

var Midgard, gir, config, mgd;
Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');

var suite = vows.describe('Midgard.Metadata');
suite.addBatch({
    'CRUD' : {
        'Create' : {
            topic : function () {
                var mdata = MidgardTest.TestBook.metadata;
                mdata.score = -1;
                mdata.authors = "Sir Lancelot and Knights";
                mdata.hidden = false;
                MidgardTest.TestBook.create();
                return MidgardTest.TestBook.metadata;
            },
            'is metadata' : function (topic) {
                assert.isObject(topic);
            },	
            'test score' : function (topic) {
                assert.equal(topic.score, -1);
            },	
            'test authors' : function (topic) {
                assert.equal(topic.authors, "Sir Lancelot and Knights");
            },
            'test hidden' : function (topic) {
                assert.isFalse(topic.hidden);
            },
            'test created' : function (topic) {
                assert.isNotNull(topic.created.year);
            }
        },
        'Update' : {
            topic : function () {
                var mdata = MidgardTest.TestBook.metadata;
                mdata.score = 1;
                mdata.authors = "Sir Lancelot and Knights and King Arthur";
                mdata.hidden = true;              
                MidgardTest.TestBook.update();
                return MidgardTest.TestBook.metadata;
            },
            'is metadata' : function (topic) {
                assert.isObject(topic);
            },	
            'test score' : function (topic) {
                assert.equal(topic.score, 1);
            },	
            'test authors' : function (topic) {
                assert.equal(topic.authors, "Sir Lancelot and Knights and King Arthur");
            },
            'test hidden' : function (topic) {
                assert.isTrue(topic.hidden);
            },
            'test revised' : function (topic) {
                assert.equal("TODO", "DONE");
            }
        },
        'Delete' : {
            topic : function () { 
                MidgardTest.TestBook.delete(false);
                return MidgardTest.TestBook.metadata;
            },
            'is deleted' : function (topic) {
                assert.isTrue(topic.deleted);
            }
        },
        'Purge BookStore' : {
            topic : function () {
                return MidgardTest.TestBookStore.purge(false)
            },
            'check error' : function (topic) {
                assert.equal(MidgardTest.cnc.get_error_string(), "MGD_ERR_OK");
            },	
            'is purged' : function (topic) {
                assert.isTrue(topic);
            }	
        }
    }
}).run();
