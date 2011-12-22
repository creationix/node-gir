var gir = require("../gir"),
    EventEmitter = require("events").EventEmitter;

gir.init();

var webkit = exports.webkit = gir.load("WebKit", "3.0");

module.exports = webkit;
