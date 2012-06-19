
var mocha = require('mocha'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');

var mdata = MidgardTest.TestBook.metadata;
mdata.score = -1;
mdata.authors = "Sir Lancelot and Knights";
mdata.hidden = false;
var published = mdata.published;
published.year = 2012;
published.month = 7;
published.day = 1; 
var datestring = "2012-07-01 00:00:00+0000";
MidgardTest.TestBook.metadata.published = published;
MidgardTest.TestBook.create();


describe('Midgard.Metadata', function() {
    
    describe('Create', function() {
    
        it('is metadata', function () {
            assert.isObject(MidgardTest.TestBook.metadata);
        });
        
        it('test score', function () {
            assert.equal(MidgardTest.TestBook.metadata.score, -1);
        });
        
        it('test authors', function () {
            assert.equal(MidgardTest.TestBook.metadata.authors, "Sir Lancelot and Knights");
        });
        
        it('test hidden', function () {
            assert.isFalse(MidgardTest.TestBook.metadata.hidden);
        });
        
        it('test created', function () {
            var year = MidgardTest.TestBook.metadata.created.year;
            assert.isNotNull(year);
            var d = new Date();
            assert.equal(year, d.getFullYear());
        });
        
        it('test creator', function () {
            var creator = MidgardTest.TestBook.metadata.creator;
            assert.isNotNull(creator);
            assert.equal(creator, "");
        });
        
        it('test revised', function () {
            var year = MidgardTest.TestBook.metadata.revised.year;
            assert.isNotNull(year);
            var d = new Date();
            assert.equal(year, d.getFullYear());
        });
        
        it('test revisor', function () {
            var revisor = MidgardTest.TestBook.metadata.revisor;
            assert.isNotNull(revisor);
            assert.equal(revisor, "");
        });
        
        it('test published', function () {
            var year = MidgardTest.TestBook.metadata.published.year;
            var month =  MidgardTest.TestBook.metadata.published.month;
            var day =  MidgardTest.TestBook.metadata.published.day;
            assert.isNotNull(year);
            assert.isNotNull(month);
            assert.isNotNull(day);
            assert.equal(year, 2012);
            assert.equal(month, 7);
            assert.equal(day, 1);
            assert.equal(MidgardTest.TestBook.metadata.published.get_string(), datestring);
        });
    });

    describe('Update', function() {
    
        it('is updated', function () {
            var mdata = MidgardTest.TestBook.metadata;
            mdata.score = 1;
            mdata.authors = "Sir Lancelot and Knights and King Arthur";
            mdata.hidden = true;              
            MidgardTest.TestBook.update();
            assert.equal(MidgardTest.cnc.get_error_string(), "MGD_ERR_OK");
            assert.isObject(MidgardTest.TestBook.metadata);
            assert.isObject(mdata);
        });

        it('test score', function () {
            assert.equal(MidgardTest.TestBook.metadata.score, 1);
        });
        
        it('test authors', function () {
            assert.equal(MidgardTest.TestBook.metadata.authors, "Sir Lancelot and Knights and King Arthur");
        });
        
        it('test hidden', function () {
            assert.isTrue(MidgardTest.TestBook.metadata.hidden);
        });
        
        it('test revised', function () {
            var revised = MidgardTest.TestBook.metadata.revised;
            assert.isNotNull(revised);
            var d = new Date();
            assert.equal(revised.year, d.getFullYear());
            assert.equal(revised.month-1, d.getMonth()); // Data,getMonth() range is 0-11
            assert.equal(revised.day, d.getDate());
        });
        
        it('test creator', function () {
            var creator = MidgardTest.TestBook.metadata.creator;
            assert.isNotNull(creator);
            assert.equal(creator, "");
        });

    });

    describe('Delete', function() {
    
        it('is deleted', function () {
            var deleted = MidgardTest.TestBook.delete(false);
            assert.equal(MidgardTest.cnc.get_error_string(), "MGD_ERR_OK");
            assert.isTrue(deleted);
            assert.isTrue(MidgardTest.TestBook.metadata.deleted);
        });
    });
    describe('Purge', function() {
    
        it('is purged', function () {
            var purged = MidgardTest.TestBook.purge(false);
            assert.equal(MidgardTest.cnc.get_error_string(), "MGD_ERR_OK");
            assert.isTrue(purged);
            assert.isTrue(MidgardTest.TestBook.metadata.deleted);
        });
    });
});
