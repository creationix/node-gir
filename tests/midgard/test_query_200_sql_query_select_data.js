
var mocha = require('mocha'),
    should = require('should');

var gir = require('../../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');
var select;
var property;

describe('Midgard.SqlQuerySelectData', function() {

    it('add column', function() {
        select = new Midgard.SqlQuerySelectData({'connection':MidgardTest.cnc});
        var column = new Midgard.SqlQueryColumn({
            'queryproperty': new Midgard.QueryProperty({'property':'a'}),
            'name':'a',
            'qualifier':'t1'
        });
        //Expect exception in case of error
        select.add_column(column)
    });

    it('get columns', function() {
        var column = new Midgard.SqlQueryColumn({
            'queryproperty': new Midgard.QueryProperty({'property':'b'}),
            'name':'b',
            'qualifier':'t2'
        });
        select.add_column(column);
        
        var column = new Midgard.SqlQueryColumn({
            'queryproperty': new Midgard.QueryProperty({'property':'c'}),
            'name':'c',
            'qualifier':'t3'
        });
        select.add_column(column);

        var columns = select.get_columns();
        columns.length.should.equal(3);
        columns[0].get_name().should.equal('a');
        columns[0].get_qualifier().should.equal('t1');
        columns[1].get_name().should.equal('b');
        columns[1].get_qualifier().should.equal('t2');
        columns[2].get_name().should.equal('c');
        columns[2].get_qualifier().should.equal('t3');
    });

    it('execute', function() {
        var sel = new Midgard.SqlQuerySelectData({'connection':MidgardTest.cnc});
        var storage = new Midgard.QueryStorage({'dbclass':'midgard_person'});
        var column = new Midgard.SqlQueryColumn({
            'queryproperty': new Midgard.QueryProperty({'property':'firstname', 'storage':storage}),
            'name':'name',
            'qualifier':'p'
        });
        sel.add_column(column);
        sel.execute();
    });

    it('get query result', function() {
        var sel = new Midgard.SqlQuerySelectData({'connection':MidgardTest.cnc});
        var storage = new Midgard.QueryStorage({'dbclass':'gir_test_book_crud'});
        var column = new Midgard.SqlQueryColumn({
            'queryproperty': new Midgard.QueryProperty({'property':'title', 'storage':storage}),
            'name':'name',
            'qualifier':'p'
        });
        sel.add_column(column);
        sel.execute();
        var result = sel.get_query_result();
        result.should.be.a('object');
    });
});
