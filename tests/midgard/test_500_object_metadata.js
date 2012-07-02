
var mocha = require('mocha'),
    should = require('should');

var gir = require('../../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');

var mgd = MidgardTest.cnc;
var TestBook = Midgard.Object.factory(mgd, "gir_test_book_store");

var mdata = TestBook.metadata;
mdata.score = -1;
mdata.authors = "Sir Lancelot and Knights";
mdata.hidden = false;
var published = mdata.published;
published.year = 2012;
published.month = 7;
published.day = 1; 
var datestring = "2012-07-01 00:00:00+0000";

describe('Midgard.Metadata', function() {
    
    describe('Create', function() {
    
        it('is created', function () {
            TestBook.metadata.published = published;
            var created = TestBook.create();
            MidgardTest.cnc.get_error_string().should.equal("MGD_ERR_OK");
            MidgardTest.cnc.get_error().should.equal(Midgard.GenericError.ok);
            created.should.equal(true);
            TestBook.metadata.should.be.a('object');
        });

        it('is metadata', function () {
            TestBook.metadata.should.be.a('object');
        });
        
        it('test score', function () {
            TestBook.metadata.score.should.be.a('number');
            TestBook.metadata.score.should.equal(-1);
        });
        
        it('test authors', function () {
            TestBook.metadata.authors.should.be.a('string');
            TestBook.metadata.authors.should.equal('Sir Lancelot and Knights');
        });
        
        it('test hidden', function () {
            TestBook.metadata.hidden.should.be.a('boolean');
            TestBook.metadata.hidden.should.equal(false);
        });
        
        it('test created', function () {
            var year = TestBook.metadata.created.year;
            var d = new Date();
            year.should.be.a('number');
            year.should.equal(d.getFullYear());
        });
        
        it('test creator', function () {
            var creator = TestBook.metadata.creator;
            creator.should.be.a('string');
            creator.should.equal('');
        });
        
        it('test revised', function () {
            var year = TestBook.metadata.revised.year;
            var d = new Date();
            year.should.be.a('number');
            year.should.equal(d.getFullYear());
        });
        
        it('test revisor', function () {
            var revisor = TestBook.metadata.revisor;
            revisor.should.be.a('string');
            revisor.should.equal('');
        });
        
        it('test published', function () {
            var year = TestBook.metadata.published.year;
            var month =  TestBook.metadata.published.month;
            var day =  TestBook.metadata.published.day;
            year.should.be.a('number');
            year.should.equal(2012);
            month.should.be.a('number');
            month.should.equal(7);
            day.should.be.a('number');
            day.should.equal(1);
            TestBook.metadata.published.get_string().should.equal(datestring);
        });
    });

    describe('Update', function() {
    
        it('is updated', function () {
            var mdata = TestBook.metadata;
            mdata.score = 1;
            mdata.authors = "Sir Lancelot and Knights and King Arthur";
            mdata.hidden = true;              
            TestBook.update();
            MidgardTest.cnc.get_error_string().should.equal("MGD_ERR_OK");
            TestBook.metadata.should.be.a('object');
        });

        it('test score', function () {
            TestBook.metadata.score.should.equal(1);
        });
        
        it('test authors', function () {
            TestBook.metadata.authors.should.equal('Sir Lancelot and Knights and King Arthur');
        });
        
        it('test hidden', function () {
            TestBook.metadata.hidden.should.equal(true);
        });
        
        it('test revised', function () {
            var revised = TestBook.metadata.revised;
            var d = new Date();
            revised.year.should.equal(d.getFullYear());
            revised.month.should.equal(d.getMonth() + 1); // Data,getMonth() range is 0-11
            revised.day.should.equal(d.getDate());
        });
        
        it('test creator', function () {
            var creator = TestBook.metadata.creator;
            creator.should.equal('');
        });

    });

    describe('Delete', function() {
    
        it('is deleted', function () {
            var deleted = TestBook.delete(false);
            MidgardTest.cnc.get_error_string().should.equal('MGD_ERR_OK');
            deleted.should.equal(true);
        });
    });
    describe('Purge', function() {
    
        it('is purged', function () {
            var purged = TestBook.purge(false);
            MidgardTest.cnc.get_error_string().should.equal('MGD_ERR_OK');
            purged.should.equal(true);
            TestBook.metadata.deleted.should.equal(true);
        });
    });
});
