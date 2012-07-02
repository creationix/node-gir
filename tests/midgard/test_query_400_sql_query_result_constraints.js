
var mocha = require('mocha'),
    should = require('should');

var gir = require('../../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');
var mgd = MidgardTest.cnc;
var select;
var result;

var book_qualifier = 'book_q';
var book_title = 'btitle';
var book_a_title = 'Book A';
var book_title_column = 'booktitle';
var book_store_id = 'book_store_id';

var book_store_qualifier = 'bookstore';

describe('Midgard.SqlQueryResult - Constraints', function() {

    beforeEach(function(done){
        var tr = new Midgard.Transaction({'connection':mgd});
        tr.begin();
        
        var sdirA = Midgard.Object.factory(mgd, 'gir_test_book_store');
        sdirA.name = 'Bookstore';
        var created = sdirA.create();
        created.should.equal(true);
        var idA = sdirA.id;

        var sA = Midgard.Object.factory(mgd, 'gir_test_book_crud');
        sA.title = book_a_title;
        sA.edition = 1;
        sA.store = idA;
        created = sA.create();
        created.should.equal(true);

        var sB = Midgard.Object.factory(mgd, 'gir_test_book_crud');
        sB.title = 'Book B';
        sB.edition = 2;
        sB.store = idA;
        created = sB.create();
        created.should.equal(true);

        var sC = Midgard.Object.factory(mgd, 'gir_test_book_crud');
        sC.title = 'Book C';
        sC.edition = 3;
        sC.store = idA;
        created = sC.create();
        created.should.equal(true);
       
        tr.commit();
        done();
    });

    afterEach(function(done){
        var tr = new Midgard.Transaction({'connection':mgd});
        tr.begin();
        
        var st = new Midgard.QueryStorage({'dbclass':'gir_test_book_crud'});
        var qs = new Midgard.QuerySelect({'connection':mgd, 'storage':st});
        qs.execute();
        var objects = qs.list_objects();
        var i = 0;
        for (i in objects) {
            objects[i].purge(false);
        }

        st = new Midgard.QueryStorage({'dbclass':'gir_test_book_store'});
        qs = new Midgard.QuerySelect({'connection':mgd, 'storage':st});
        qs.execute();
        var objects = qs.list_objects();
        var i = 0;
        for (i in objects) {
            objects[i].purge(false);
        }

        tr.commit();
        done();
    })

    it('execute invalid query', function() {
        select = new Midgard.SqlQuerySelectData({'connection':MidgardTest.cnc});
        var storage = new Midgard.QueryStorage({'dbclass':'midgard_person'});
        var column = new Midgard.SqlQueryColumn({
            'queryproperty': new Midgard.QueryProperty({'property':'firstname', 'storage':storage}),
            'name':'name',
            'qualifier':'p'
        });
        select.add_column(column);
        select.set_constraint(
            new Midgard.SqlQueryConstraint(
                {'column': new Midgard.SqlQueryColumn({
                    'queryproperty': new Midgard.QueryProperty({'property':'title'}),
                    'qualifier': book_qualifier
                    }),
                'operator':'=',
                'holder': new Midgard.QueryValue.create_with_value(book_a_title)
                })
            );
        try {
            select.execute();
        } catch (err) {
            err.message.should.not.be.equal('');
            // https://github.com/creationix/node-gir/issues/19
            //console.log(err);
            //err.should.be.an.instanceof(GObject.Error)
            //err.code.should.equal(Midgard.ValidationError.TYPE_INVALID);
        }
    });

    it('execute', function() {
        select = new Midgard.SqlQuerySelectData({'connection':MidgardTest.cnc});
        var storage = new Midgard.QueryStorage({'dbclass':'midgard_person'});
        var column = new Midgard.SqlQueryColumn({
            'queryproperty': new Midgard.QueryProperty({'property':'firstname', 'storage':storage}),
            'name':'name',
            'qualifier':'p'
        });
        select.add_column(column);
        select.execute();
    });
});
