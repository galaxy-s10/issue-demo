const enigo = require('node-enigo');

console.log(enigo);
let num = 0;
let timer;
timer = setInterval(() => {
  num += 1;
  if (num > 3) {
    clearInterval(timer);
    return;
  }
  enigo.moveMouse(100, 100);
}, 2000);
