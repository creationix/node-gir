var notify = require("./notify");
notify.init("ich");

var n = new notify.Notification();
n.__call__("new", "1", "2", "3");
