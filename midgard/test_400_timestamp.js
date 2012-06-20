
var vows   = require('vows'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

var GObject = gir.load('GObject');

var Midgard, gir, config, mgd;
Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');

var suite = vows.describe('Midgard.Timestamp');
suite.addBatch({
    'ISO 8601' : {
        'new from iso' : {
            topic : "2012-06-07 14:35:00+0000",
            'is new' : function (topic) {
                assert.equal("TODO", "DONE");
                //var ts = Midgard.Timestamp.new_from_iso8601(topic);
                //assert.isObject(ts);
                //assert.equal(ts.get_string(), "2012-06-07 14:35:00+0000");
            }	
        }
    },
    'Get Default' : {
        'Year' : {
            topic : MidgardTest.TimeStamp,
            'is 0' : function (topic) {
                assert.isNumber(topic.year);
                assert.equal(topic.year, 0);
            }	
        },
        'Month' : {
            topic : MidgardTest.TimeStamp,
            'is 0' : function (topic) {
                assert.isNumber(topic.month);
                assert.equal(topic.month, 0);
            }	
        },
        'Day' : {
            topic : MidgardTest.TimeStamp,
            'is 0' : function (topic) { 
                assert.isNumber(topic.day);
                assert.equal(topic.day, 0);
            }	
        },
        'Hour' : {
            topic : MidgardTest.TimeStamp,
            'is 0' : function (topic) {
                assert.isNumber(topic.hour);
                assert.equal(topic.hour, 0);
            }	
        },
        'Minute' : {
            topic : MidgardTest.TimeStamp,
            'is 0' : function (topic) {
                assert.isNumber(topic.minute);
                assert.equal(topic.minute, 0);
            }	
        },
        'Second' : {
            topic : MidgardTest.TimeStamp,
            'is 0' : function (topic) {
                assert.isNumber(topic.second);
                assert.equal(topic.second, 0);
            }	
        },
        'get_string' : {
            topic : MidgardTest.TimeStamp,
            'is 0' : function (topic) {
                assert.isString(topic.get_string());
                assert.equal(topic.get_string(), "0000-00-00 00:00:00+0000");
            }	
        }
    },
    'Set' : {
        'Year' : {
            topic : MidgardTest.TimeStamp,
            'is 2012' : function (topic) {
                topic.year = 2012;
                assert.isNumber(topic.year);
                assert.equal(topic.year, 2012);
            }	
        },
        'Month' : {
            topic : MidgardTest.TimeStamp,
            'is 6' : function (topic) {
                topic.month = 6;
                assert.isNumber(topic.month);
                assert.equal(topic.month, 6);
            }	
        },
        'Day' : {
            topic : MidgardTest.TimeStamp,
            'is 7' : function (topic) {
                topic.day = 7;
                assert.isNumber(topic.day);
                assert.equal(topic.day, 7);
            }	
        },
        'Hour' : {
            topic : MidgardTest.TimeStamp,
            'is 14' : function (topic) {
                topic.hour = 14;
                assert.isNumber(topic.hour);
                assert.equal(topic.hour, 14);
            }	
        },
        'Minute' : {
            topic : MidgardTest.TimeStamp,
            'is 35' : function (topic) {
                topic.minute = 35;
                assert.isNumber(topic.minute);
                assert.equal(topic.minute, 35);
            }	
        },
        'Second' : {
            topic : MidgardTest.TimeStamp,
            'is 0' : function (topic) {
                topic.second = 0;
                assert.isNumber(topic.second);
                assert.equal(topic.second, 0);
            }	
        }
    },
    'Get' : {
        'Year' : {
            topic : MidgardTest.TimeStamp,
            'is 2012' : function (topic) {
                assert.isNumber(topic.year);
                assert.equal(topic.year, 2012);
            }	
        },
        'Month' : {
            topic : MidgardTest.TimeStamp,
            'is 6' : function (topic) {
                assert.isNumber(topic.month);
                assert.equal(topic.month, 6);
            }	
        },
        'Day' : {
            topic : MidgardTest.TimeStamp,
            'is 7' : function (topic) { 
                assert.isNumber(topic.day);
                assert.equal(topic.day, 7);
            }	
        },
        'Hour' : {
            topic : MidgardTest.TimeStamp,
            'is 14' : function (topic) {
                assert.isNumber(topic.hour);
                assert.equal(topic.hour, 14);
            }	
        },
        'Minute' : {
            topic : MidgardTest.TimeStamp,
            'is 35' : function (topic) {
                assert.isNumber(topic.minute);
                assert.equal(topic.minute, 35);
            }	
        },
        'Second' : {
            topic : MidgardTest.TimeStamp,
            'is 0' : function (topic) {
                assert.isNumber(topic.second);
                assert.equal(topic.second, 0);
            }	
        },
        'get_string' : {
            topic : MidgardTest.TimeStamp,
            'is 0' : function (topic) {
                assert.isString(topic.get_string());
                assert.equal(topic.get_string(), "2012-06-07 14:35:00+0000");
            }	
        }
    }
}).run();
