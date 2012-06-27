
var mocha = require('mocha'),
    should = require('should');

var gir = require('../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');

describe('Midgard.Object CRUD', function() {

    describe('Create', function() {
       
        var book = MidgardTest.TestBook;

        it('is created', function() {
            var created = book.create();
            created.should.be.a('boolean');
            created.should.equal(true);
        });

        it('check error code', function() {
            var error_code = MidgardTest.cnc.get_error();
            error_code.should.equal(Midgard.GenericError.ok);
        });

        it('check error string', function() {
            var error_string = MidgardTest.cnc.get_error_string();
            error_string.should.equal("MGD_ERR_OK");
        });

        it('is valid guid', function() {
            var valid_guid = true;
            valid_guid.should.equal(Midgard.is_guid(book.guid));
        });

    });

    describe('Update', function() {
       
        var book = MidgardTest.TestBook;
        book.title = 'The Holy Grail 2';

        it('is updated', function() {
            var updated = book.update();
            updated.should.be.a('boolean');
            updated.should.equal(true);
        });

        it('check error code', function() {
            var error_code = MidgardTest.cnc.get_error();
            error_code.should.equal(Midgard.GenericError.ok);
        });

        it('check error string', function() {
            var error_string = MidgardTest.cnc.get_error_string();
            error_string.should.equal("MGD_ERR_OK");
        });

        it('is valid guid', function() {
            var valid_guid = true;
            valid_guid.should.equal(Midgard.is_guid(book.guid));
        });
    });

    describe('Delete', function() {
       
        var book = MidgardTest.TestBook;

        it('is deleted', function() {
            var deleted = book.delete(false);
            deleted.should.be.a('boolean');
            deleted.should.equal(true);
        });

        it('check error code', function() {
            var error_code = MidgardTest.cnc.get_error();
            error_code.should.equal(Midgard.GenericError.ok);
        });

        it('check error string', function() {
            var error_string = MidgardTest.cnc.get_error_string();
            error_string.should.equal("MGD_ERR_OK");
        });
    });

    describe('Purge book', function() {
       
        var book = MidgardTest.TestBook;

        it('is purged', function() {
            var purged = book.purge(false);
            purged.should.be.a('boolean');
            purged.should.equal(true);
        });

        it('check error code', function() {
            var error_code = MidgardTest.cnc.get_error();
            error_code.should.equal(Midgard.GenericError.ok);
        });

        it('check error string', function() {
            var error_string = MidgardTest.cnc.get_error_string();
            error_string.should.equal("MGD_ERR_OK");
        });
    });

    describe('Purge bookstore', function() {
       
        var book = MidgardTest.TestBookStore;

        it('is purged', function() {
            var purged = book.purge(false);
            purged.should.be.a('boolean');
            purged.should.equal(true);
        });

        it('check error code', function() {
            var error_code = MidgardTest.cnc.get_error();
            error_code.should.equal(Midgard.GenericError.ok);
        });

        it('check error string', function() {
            var error_string = MidgardTest.cnc.get_error_string();
            error_string.should.equal("MGD_ERR_OK");
        });
    });

});
