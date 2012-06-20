
var mocha = require('mocha'),
    assert = require('assert');

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
            assert.isObject(user);
        });
    });

    describe('Create', function() {
    
        it('is created', function () {
            var user = new Midgard.User({'connection':MidgardTest.cnc, 'login':'John', 'authtype':'Plaintext', 'active':true});
            assert.isTrue(user.create());
        });
 
        it('is not created, duplicated', function () {
            var user = new Midgard.User({'connection':MidgardTest.cnc, 'login':'John', 'authtype':'Plaintext', 'active':true});
            assert.isFalse(user.create());
        });       
    });

    describe('Update', function() {
    
        it('is updated', function () {
            var user = new Midgard.User({'connection':MidgardTest.cnc, 'login':'Alice', 'authtype':'Plaintext', 'active':true});
            assert.isTrue(user.create());
            user.active = false;
            assert.isTrue(user.update());
        }); 
    });
});
