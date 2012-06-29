
var mocha = require('mocha'),
            should = require('should');

var gir = require('../../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');

// Test G_TYPE_BOXED
// metadata.created is C structure hold in GValue of boxed type

describe('Midgard.TimeStamp (Boxed)', function() {

    describe('Get default values', function() {

        it ('year', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.year.should.be.a('number');
            v.year.should.equal(1);
        });

        it ('month', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.month.should.be.a('number');
            v.month.should.equal(1);
        });

        it ('day', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.day.should.be.a('number');
            v.day.should.equal(1);
        });

        it ('hour', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.hour.should.be.a('number');
            v.hour.should.equal(0);
        });

        it ('minute', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.minute.should.be.a('number');
            v.minute.should.equal(0);
        });

        it ('second', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.second.should.be.a('number');
            v.second.should.equal(0);
        });

        it ('get string', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.get_string().should.be.a('string');
            v.get_string().should.equal('0001-01-01 00:00:00+0000');
        });
    });

    describe('Set year', function() {

        it ('created', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.year = 2012;
            v.year.should.be.a('number');
            v.year.should.equal(2012);
        });

        it ('revised', function() {
            var v = MidgardTest.TestBook.metadata.revised;
            v.year = 2013;
            v.year.should.be.a('number');
            v.year.should.equal(2013);
        });

        it ('published', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.year = 2014;
            v.year.should.be.a('number');
            v.year.should.equal(2014);
        });

    });

    describe('Set month', function() {

        it ('created', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.month = 1;
            v.month.should.be.a('number');
            v.month.should.equal(1);
        });

        it ('revised', function() {
            var v = MidgardTest.TestBook.metadata.revised;
            v.month = 2;
            v.month.should.be.a('number');
            v.month.should.equal(2);
        });

        it ('published', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.month = 3;
            v.month.should.be.a('number');
            v.month.should.equal(3);
        });

    });

    describe('Set day', function() {

        it ('created', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.day = 1;
            v.day.should.be.a('number');
            v.day.should.equal(1);
        });

        it ('revised', function() {
            var v = MidgardTest.TestBook.metadata.revised;
            v.day = 2;
            v.day.should.be.a('number');
            v.day.should.equal(2);
        });

        it ('published', function() {
            var v = MidgardTest.TestBook.metadata.published;
            v.day = 3;
            v.day.should.be.a('number');
            v.day.should.equal(3);
        });
    });

    describe('Set hour', function() {

        it ('created', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.hour = 1;
            v.hour.should.be.a('number');
            v.hour.should.equal(1);
        });

        it ('revised', function() {
            var v = MidgardTest.TestBook.metadata.revised;
            v.hour = 2;
            v.hour.should.be.a('number');
            v.hour.should.equal(2);
        });

        it ('published', function() {
            var v = MidgardTest.TestBook.metadata.published;
            v.hour = 3;
            v.hour.should.be.a('number');
            v.hour.should.equal(3);
        });
    });

    describe('Set minute', function() {

        it ('created', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.minute = 1;
            v.minute.should.be.a('number');
            v.minute.should.equal(1);
        });

        it ('revised', function() {
            var v = MidgardTest.TestBook.metadata.revised;
            v.minute = 2;
            v.minute.should.be.a('number');
            v.minute.should.equal(2);
        });

        it ('published', function() {
            var v = MidgardTest.TestBook.metadata.published;
            v.minute = 3;
            v.minute.should.be.a('number');
            v.minute.should.equal(3);
        });
    });

    describe('Set second', function() {

        it ('created', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.second = 1;
            v.second.should.be.a('number');
            v.second.should.equal(1);
        });

        it ('revised', function() {
            var v = MidgardTest.TestBook.metadata.revised;
            v.second = 2;
            v.second.should.be.a('number');
            v.second.should.equal(2);
        });

        it ('published', function() {
            var v = MidgardTest.TestBook.metadata.published;
            v.second = 3;
            v.second.should.be.a('number');
            v.second.should.equal(3);
        });
    });

    describe('Get string', function() {

        it ('created', function() {
            var v = MidgardTest.TestBook.metadata.created;
            v.year = 2012;
            v.month = 6;
            v.day = 7;
            v.hour = 14;
            v.minute = 35;
            v.second = 0;
            v.get_string().should.be.a('string');
            v.get_string().should.equal('2012-06-07 14:35:00+0000');
        });
    });
});
