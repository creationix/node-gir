
var mocha = require('mocha'),
        should = require('should');

var gir = require('../../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');

describe('Midgard.TimeStamp', function() {
    
    it('new from iso', function() {
        // It should be resolved ni midgard-core
        // e.g. Midgard.TimeStamp.create_from_iso....
        //var datetime = "2012-06-07 14:35:00+0000";
        //var ts = Midgard.TimeStamp.new_from_iso8601(datetime);
        //ts.get_string().should.equal(datetime);
    });

    describe('Get default values', function() {
        
        it ('year', function() { 
            var v = MidgardTest.TimeStamp;
            v.year.should.be.a('number');
            v.year.should.equal(0);
        });

        it ('month', function() { 
            var v = MidgardTest.TimeStamp;
            v.month.should.be.a('number');
            v.month.should.equal(0);
        });

        it ('day', function() { 
            var v = MidgardTest.TimeStamp;
            v.day.should.be.a('number');
            v.day.should.equal(0);
        });

        it ('hour', function() { 
            var v = MidgardTest.TimeStamp;
            v.hour.should.be.a('number');
            v.hour.should.equal(0);
        });

        it ('minute', function() { 
            var v = MidgardTest.TimeStamp;
            v.minute.should.be.a('number');
            v.minute.should.equal(0);
        });

        it ('second', function() { 
            var v = MidgardTest.TimeStamp;
            v.second.should.be.a('number');
            v.second.should.equal(0);
        });

        it ('get string', function() { 
            var v = MidgardTest.TimeStamp;
            v.get_string().should.be.a('string');
            v.get_string().should.equal('0000-00-00 00:00:00+0000');
        });
    });

    describe('Set values', function() {

        it ('year', function() { 
            var v = MidgardTest.TimeStamp;
            v.year = 2012;
            v.year.should.be.a('number');
            v.year.should.equal(2012);
        });

        it ('month', function() { 
            var v = MidgardTest.TimeStamp;
            v.month = 6;
            v.month.should.be.a('number');
            v.month.should.equal(6);
        });

        it ('day', function() { 
            var v = MidgardTest.TimeStamp;
            v.day = 7;
            v.day.should.be.a('number');
            v.day.should.equal(7);
        });

        it ('hour', function() { 
            var v = MidgardTest.TimeStamp;
            v.hour = 14;
            v.hour.should.be.a('number');
            v.hour.should.equal(14);
        });

        it ('minute', function() { 
            var v = MidgardTest.TimeStamp;
            v.minute = 35;
            v.minute.should.be.a('number');
            v.minute.should.equal(35);
        });

        it ('second', function() { 
            var v = MidgardTest.TimeStamp;
            v.second = 0;
            v.second.should.be.a('number');
            v.second.should.equal(0);
        });

        it ('get string', function() { 
            var v = MidgardTest.TimeStamp;
            v.get_string().should.be.a('string');
            v.get_string().should.equal('2012-06-07 14:35:00+0000');
        });

    });

});

