
var vows   = require('vows'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

var GObject = gir.load('GObject');

var Midgard, gir, config, mgd;
Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');

// Test G_TYPE_BOXED
// metadata.created is C structure hold in GValue of boxed type

var suite = vows.describe('Midgard.Timestamp (Boxed)');
suite.addBatch({
    'Get Default' : {
        'Year' : {
            'Created' : {
                topic : MidgardTest.TestBook.metadata.created,
                'is 1' : function (topic) {
                    assert.isNumber(topic.year);
                    assert.equal(topic.year, 1);
                }
            },
             'Revised' : {
                topic : MidgardTest.TestBook.metadata.revised,
                'is 1' : function (topic) {
                    assert.isNumber(topic.year);
                    assert.equal(topic.year, 1);
                }
            }
        },
        'Month' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 1' : function (topic) {
                assert.isNumber(topic.month);
                assert.equal(topic.month, 1);
            }	
        },
        'Day' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 1' : function (topic) { 
                assert.isNumber(topic.day);
                assert.equal(topic.day, 1);
            }	
        },
        'Hour' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 0' : function (topic) {
                assert.isNumber(topic.hour);
                assert.equal(topic.hour, 0);
            }	
        },
        'Minute' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 0' : function (topic) {
                assert.isNumber(topic.minute);
                assert.equal(topic.minute, 0);
            }	
        },
        'Second' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 0' : function (topic) {
                assert.isNumber(topic.second);
                assert.equal(topic.second, 0);
            }	
        },
        'get_string' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 0' : function (topic) {
                assert.isString(topic.get_string());
                assert.equal(topic.get_string(), "0001-01-01 00:00:00+0000");
            }	
        }
    },
    'Set Year' : {
        'Created' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 2012' : function (topic) {
                topic.year = 2012;
                assert.isNumber(topic.year);
                assert.equal(topic.year, 2012);
            }	
        },
        'Revised' : {
            topic : MidgardTest.TestBook.metadata.revised,
            'is 2013' : function (topic) {
                topic.year = 2013;
                assert.isNumber(topic.year);
                assert.equal(topic.year, 2013);
            }	
        },
        'Published' : {
            topic : MidgardTest.TestBook.metadata.published,
            'is 2014' : function (topic) {
                topic.year = 2014;
                assert.isNumber(topic.year);
                assert.equal(topic.year, 2014);
            }	
        }
    },
    'Set & Get' : {
        'Year' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 2012' : function (topic) {
                topic.year = 2012;
                assert.isNumber(topic.year);
                assert.equal(topic.year, 2012);
            }	
        },
        'Month' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 6' : function (topic) {
                topic.month = 6;
                assert.isNumber(topic.month);
                assert.equal(topic.month, 6);
            }	
        },
        'Day' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 7' : function (topic) {
                topic.day = 7;
                assert.isNumber(topic.day);
                assert.equal(topic.day, 7);
            }	
        },
        'Hour' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 14' : function (topic) {
                topic.hour = 14;
                assert.isNumber(topic.hour);
                assert.equal(topic.hour, 14);
            }	
        },
        'Minute' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 35' : function (topic) {
                topic.minute = 35;
                assert.isNumber(topic.minute);
                assert.equal(topic.minute, 35);
            }	
        },
        'Second' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 0' : function (topic) {
                topic.second = 0;
                assert.isNumber(topic.second);
                assert.equal(topic.second, 0);
            }	
        },
        'get_string' : {
            topic : MidgardTest.TestBook.metadata.created,
            'is 0' : function (topic) {
                topic.year = 2012;
                topic.month = 6;
                topic.day = 7;
                topic.hour = 14;
                topic.minute = 35;
                topic.second = 0;
                assert.isString(topic.get_string());
                assert.equal(topic.get_string(), "2012-06-07 14:35:00+0000");
            }	
        }
    }
}).run();
