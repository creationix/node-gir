
var mocha = require('mocha'),
    should = require('should');

var gir = require('../../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');

describe('Midgard.User', function() {
   
    describe('Instance', function() {
        it('is instance', function() {
            var user = new Midgard.User({'connection':MidgardTest.cnc, 'login':'John', 'authtype':'Plaintext', 'active':true});
            user.should.be.a('object');
        });
    });

    describe('John', function() {
    
        it('is created', function () {
            var user = new Midgard.User({'connection':MidgardTest.cnc, 'login':'John', 'authtype':'Plaintext', 'active':true});
            var created = user.create();
            MidgardTest.cnc.get_error_string().should.equal('MGD_ERR_OK');
            MidgardTest.cnc.get_error().should.equal(Midgard.GenericError.ok);
            created.should.equal(true);
        });
 
        it('is not created, duplicated', function () {
            var user = new Midgard.User({'connection':MidgardTest.cnc, 'login':'John', 'authtype':'Plaintext', 'active':true});
            var created = user.create();
            MidgardTest.cnc.get_error_string().should.equal('Object already exist.');
            MidgardTest.cnc.get_error().should.equal(Midgard.GenericError.duplicate);
            created.should.equal(false);
        });       
    });

    describe('Alice', function() {
    
        it('is updated', function () {
            var user = new Midgard.User({'connection':MidgardTest.cnc, 'login':'Alice', 'authtype':'Plaintext', 'active':true});
            var created = user.create();
            MidgardTest.cnc.get_error_string().should.equal('MGD_ERR_OK');
            MidgardTest.cnc.get_error().should.equal(Midgard.GenericError.ok);
            created.should.equal(true)
            user.active = false;
            var updated = user.update();
            MidgardTest.cnc.get_error_string().should.equal('MGD_ERR_OK');
            MidgardTest.cnc.get_error().should.equal(Midgard.GenericError.ok);
            updated.should.equal(true);
        }); 
    });
});
