//docs: http://developer.gnome.org/libnotify/0.7/NotifyNotification.html
var notify = require('./notify');

console.log(notify.init('notify_test.js sample application'));

var n = new notify.Notification({'summary':'a'});

n.update('Notify Test', 'This is a test notification message via Node.JS.');
n.show();

setTimeout(
  function () {
    n.update('Notification Update', 'The status has been updated!');
    n.show();
    setTimeout(
      function () {
        n.update('Ethan', 'This message will self-destruct in 5 seconds.');
        n.show();
        setTimeout(
          function () {
            n.close();
            console.log('Adios!');
          }, 5000
        );
      }, 4000
    );
  }, 3000
);
