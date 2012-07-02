
var mocha = require('mocha'),
    should = require('should');

var gir = require('../../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');

var MidgardTest = require('./midgard_connection');

describe('Midgard.Storage', function() {

    it('create base storage', function() {
        var created = Midgard.Storage.create_base_storage(MidgardTest.cnc);
        created.should.equal(true);
    });

    it('create MgdSchema storage', function() {
        var type = GObject.type_from_name('MidgardObject');
        var children = GObject.type_children(type);
        var i = 0;
        for (i in children) {
            console.log("Create " + GObject.type_name(children[i]) + " storage");
            var created = Midgard.Storage.create(MidgardTest.cnc, GObject.type_name(children[i]));
            created.should.equal(true);
        }
    });
   
    it('create DBObject storage', function() {
        var type = GObject.type_from_name('MidgardDBObject');
        var children = GObject.type_children(type);
        var i = 0;
        var ignored = {'MidgardMetadata':1, 'MidgardObject':1, 'MidgardView':1};
        for (i in children) {
            var typename = GObject.type_name(children[i]);
            if (ignored[typename]) {
                continue;
            }
            //console.log("Create " + GObject.type_name(children[i]) + " storage");
            var created = Midgard.Storage.create(MidgardTest.cnc, GObject.type_name(children[i]));
            created.should.equal(true);
        }
    });
         
    it('update MgdSchema storage', function() {
        var type = GObject.type_from_name('MidgardObject');
        var children = GObject.type_children(type);
        var i = 0;
        for (i in children) {
            //console.log("Create " + GObject.type_name(children[i]) + " storage");
            var updated = Midgard.Storage.update(MidgardTest.cnc, GObject.type_name(children[i]));
            updated.should.equal(true);
        }
    });
    
    it('update DBObject storage', function() {
        var type = GObject.type_from_name('MidgardDBObject');
        var children = GObject.type_children(type);
        var i = 0;
        var ignored = {'MidgardMetadata':1, 'MidgardObject':1, 'MidgardView':1};
        for (i in children) {
            var typename = GObject.type_name(children[i]);
            if (ignored[typename]) {
                continue;
            }
            //console.log("Create " + GObject.type_name(children[i]) + " storage");
            var updated = Midgard.Storage.create(MidgardTest.cnc, GObject.type_name(children[i]));
            updated.should.equal(true);
        }
    }); 
}); 
