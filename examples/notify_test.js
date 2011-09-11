var notify = require("./notify");

console.log(notify.init("ich"));

var n = new notify.Notification();
var created = n.__call__("new", "a", "a", "a", "a");

for(var k in n) {
    console.log(k);
}

console.log(notify.Notification.__methods__);
