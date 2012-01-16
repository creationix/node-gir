var gir = require("../gir");

gir.init();
console.log(gir.searchPath());

gir.import("Gtk");

console.log(gir.loadedNamespaces());

console.log(gir.getDependencies("Gtk"));

console.log(gir.getVersions("Gtk"));

console.log(gir.isRegistered("Gtk", "3.0"));
