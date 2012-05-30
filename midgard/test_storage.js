
var vows   = require('vows'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

var GObject = require('./gobject');

var Midgard, gir, config, mgd;
Midgard = gir.load('Midgard');
Midgard.init();

var mgd = require('./midgard_connection');

var suite = vows.describe('Midgard.Storage');
suite.addBatch({
    'Storage' : {
        'Create base' : {
            topic : function () {
                return Midgard.Storage.create_base_storage(mgd.cnc);
            },
            'created' : function (topic) {
                assert.isTrue (topic)
            }	
        },
        'Create MgdSchema' : {
            topic : function () {
                var type = GObject.type_from_name('MidgardObject');
                children = GObject.type_children(type);
                for (i in children) {
                    //console.log("Create " + GObject.type_name(children[i]) + " storage");
                    if (!Midgard.Storage.create(mgd.cnc, GObject.type_name(children[i]))) {
                        return false;
                    }
                }
                return true;
            },
            'created' : function (topic) {
                assert.isTrue (topic)
            }	
        },
        'Create DBObject' : {
            topic : function () {
                var type = GObject.type_from_name('MidgardDBObject');
                children = GObject.type_children(type);
                var ignored = {'MidgardMetadata':1, 'MidgardObject':1, 'MidgardView':1};
                for (i in children) {
                    var typename = GObject.type_name(children[i]);
                    if (ignored[typename]) {
                        continue;
                    }
                    //console.log("Create " + GObject.type_name(children[i]) + " storage");
                    if (!Midgard.Storage.create(mgd.cnc, GObject.type_name(children[i]))) {
                        return false;
                    }
                }
                return true;
            },
            'created' : function (topic) {
                assert.isTrue (topic)
            }	
        },
        'Update MgdSchema' : {
            topic : function () {
                var type = GObject.type_from_name('MidgardObject');
                children = GObject.type_children(type);
                for (i in children) {
                    //console.log("Create " + GObject.type_name(children[i]) + " storage");
                    if (!Midgard.Storage.update(mgd.cnc, GObject.type_name(children[i]))) {
                        return false;
                    }
                }
                return true;
            },
            'created' : function (topic) {
                assert.isTrue (topic)
            }	
        },
        'Update DBObject' : {
            topic : function () {
                var type = GObject.type_from_name('MidgardDBObject');
                children = GObject.type_children(type);
                var ignored = {'MidgardMetadata':1, 'MidgardObject':1, 'MidgardView':1};
                for (i in children) {
                    var typename = GObject.type_name(children[i]);
                    if (ignored[typename]) {
                        continue;
                    }
                    //console.log("Create " + GObject.type_name(children[i]) + " storage");
                    if (!Midgard.Storage.update(mgd.cnc, GObject.type_name(children[i]))) {
                        return false;
                    }
                }
                return true;
            },
            'created' : function (topic) {
                assert.isTrue (topic)
            }	
        }
    }
}).run();
