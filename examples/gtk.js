var gir = require("../gir"),
    EventEmitter = require("events").EventEmitter;

gir.init();

var gtk = exports.gtk = gir.load("Gtk");

module.exports = gtk;
