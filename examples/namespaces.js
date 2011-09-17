var gir = require("../gir");

console.log(gir.searchPath());

gir.load("Gtk");

console.log(gir.loadedNamespaces());

console.log(gir.getDependencies("Gtk"));

console.log(gir.getVersions("Gtk"));

console.log(gir.isRegistered("Gtk", "3.0"));
