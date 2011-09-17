var gir = require("../gir"),
    notify = gir.load("Notify");

console.log(notify.init("ich"));

var n = new notify.Notification();
