
var gir = require('../../gir');
gir.init();

var Midgard, gir, config, mgd;
Midgard = gir.load('Midgard');
Midgard.init();

config = new Midgard.Config();
config.read_file_at_path(__dirname + '/test_SQLITE.conf');

mgd = new Midgard.Connection();
mgd.open_config(config);

Midgard.Storage.create_base_storage(mgd);

var test_midgard_bookstore = Midgard.Object.factory(mgd, "gir_test_book_store");
test_midgard_bookstore.name = "BookStore";
//test_midgard_bookstore.create();

var test_midgard_book = Midgard.Object.factory(mgd, "gir_test_book_crud");
test_midgard_book.title = "The Holly Grail";
test_midgard_book.author = "Sir Lancelot";
test_midgard_book.price = 99.99;
test_midgard_book.serial = Midgard.Guid.new(mgd);
test_midgard_book.edition = 1;
test_midgard_book.sold = false;
test_midgard_book.description = "The true story of white rabbit";
test_midgard_book.store = test_midgard_bookstore.id;

exports.TestBookStore = test_midgard_bookstore;
exports.TestBook = test_midgard_book;
exports.cnc = mgd;
exports.TimeStamp = new Midgard.Timestamp();
