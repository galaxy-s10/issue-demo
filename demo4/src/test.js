// 用bindings包加载c++模块，也可以直接引入 let addon = require('./build/Release/hello');
let addon = require('bindings')('hello');

let x = 100;
let y = 200;
console.log('js的值', x, y);
let num = 0;
let timer;
timer = setInterval(() => {
  num += 1;
  if (num > 3) {
    clearInterval(timer);
    return;
  }
  console.log(addon.moveMouse(x, y));
}, 2000);
