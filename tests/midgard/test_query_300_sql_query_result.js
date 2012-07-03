
var mocha = require('mocha'),
    should = require('should');

var gir = require('../../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');
var select;
var result;

describe('Midgard.SqlQueryResult', function() {

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

    it('get rows', function() {
        result = select.get_query_result();
        result.should.be.a('object');
        var rows = result.get_rows();
        Array.isArray(rows).should.equal(true);
        rows.length.should.equal(1);
    });

    it('get columns', function() {
        result = select.get_query_result();
        result.should.be.a('object');
        var columns = result.get_columns();
        Array.isArray(columns).should.equal(true);
        columns.length.should.equal(1);
    });

    it('get columns qualifier', function() {
        result = select.get_query_result();
        result.should.be.a('object');
        var columns = result.get_columns();
        columns.length.should.equal(1);
        var i = 0;
        for (i in columns) {
            columns[i].get_qualifier().should.equal('p');
        }
    });

    it('get columns name', function() {
        result = select.get_query_result();
        result.should.be.a('object');
        var columns = result.get_columns();
        columns.length.should.equal(1);
        columns[0].get_name().should.equal('name');
    });

    it('get columns property name', function() {
        result = select.get_query_result();
        result.should.be.a('object');
        var columns = result.get_columns();
        columns.length.should.equal(1);
        var query_prop = columns[0].get_query_property();
        query_prop['property'].should.equal('firstname');
    });
});
