
var vows   = require('vows'),
    assert = require('assert');

var gir = require('../gir');
gir.init();

var Midgard, gir, config, mgd;
Midgard = gir.load('Midgard');
Midgard.init();

config = new Midgard.Config();
config.read_file_at_path('test_SQLITE.conf');
//config.__set_property__("dbdir", "/tmp");
//config.__set_property__("dbtype", "SQLite");
//config.__set_property__("database", "node_gir");

mgd = new Midgard.Connection();
mgd.open_config(config);

Midgard.Storage.create_base_storage(mgd);

var test_midgard_bookstore = Midgard.Object.factory(mgd, "gir_test_book_store");
test_midgard_bookstore.set_property("name", "BookStore");
test_midgard_bookstore.create();

console.log(test_midgard_bookstore.id);

var test_midgard_book = Midgard.Object.factory(mgd, "gir_test_book_crud");
test_midgard_book.set_property("title", "The Holly Grail");
test_midgard_book.set_property("author", "Sir Lancelot");
test_midgard_book.set_property("price", 99.99);
test_midgard_book.set_property("serial", Midgard.Guid.new(mgd));
test_midgard_book.set_property("edition", 1);
test_midgard_book.set_property("sold", false)
test_midgard_book.set_property("description", "The true story of white rabbit")
//test_midgard_book.set_property("store", test_midgard_bookstore.id)

exports.midgard_bookstore = test_midgard_bookstore;
exports.midgard_book = test_midgard_book;
exports.cnc = mgd;
