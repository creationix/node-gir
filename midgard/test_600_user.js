
var mocha = require('mocha'),
    should = require('should');

var gir = require('../gir');
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
            created.should.equal(true);
        });
 
        it('is not created, duplicated', function () {
            var user = new Midgard.User({'connection':MidgardTest.cnc, 'login':'John', 'authtype':'Plaintext', 'active':true});
            var created = user.create();
            created.should.equal(true);
        });       
    });

    describe('Alice', function() {
    
        it('is updated', function () {
            var user = new Midgard.User({'connection':MidgardTest.cnc, 'login':'Alice', 'authtype':'Plaintext', 'active':true});
            var created = user.create();
            created.should.equal(true)
            user.active = false;
            var updated = user.update();
            updated.should.equal(true);
        }); 
    });
});
