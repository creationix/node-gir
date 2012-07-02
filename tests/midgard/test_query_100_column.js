
var mocha = require('mocha'),
    should = require('should');

var gir = require('../../gir');
gir.init();

var GObject = gir.load('GObject');
var Midgard = gir.load('Midgard');
Midgard.init();

var MidgardTest = require('./midgard_connection');
var column;
var property;

describe('Midgard.SqlQueryColumn', function() {

    it('is instance', function() {
        property = new Midgard.QueryProperty({'property':'title'})
        column = new Midgard.SqlQueryColumn({'queryproperty':property, 'name':'The title', 'qualifier':'t1'});
        column.should.be.a('object');
        //column.should.be.an.instanceof(Midgard.SqlQueryColumn);
    });

    it('has queryproperty', function() {
        var prop = column.get_query_property();
        //prop.should.equal(prop);
        prop.property.should.equal(property.property);
    });

    it('has name', function() {
        var name = column.get_name();
        name.should.equal('The title');
    });

    it('has qualifier', function() {
        var q = column.get_qualifier();
        q.should.equal('t1');
    });

});
