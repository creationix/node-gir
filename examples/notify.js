//docs: http://developer.gnome.org/libnotify/0.7/NotifyNotification.html
var gir = require('../gir')
 ,  notify = gir.load('Notify');
 
console.log(notify.init('Notify Example'));

var n = new notify.Notification();
var created = n.__call__('new', 'a', 'a', 'a', 'a');

for(var k in n) {
    console.log(k);
}

console.log(notify.Notification.__methods__);

n.update('Notify Example', 'This is an example notification from Node.JS.');
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
            console.log('Adios!');
            n.close();
          }, 5000
        );
      }, 4000
    );
  }, 3000
);
