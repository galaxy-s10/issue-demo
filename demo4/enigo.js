import { moveMouseAbs, moveMouseRel } from '@enfpdev/napi-rs-enigo';
// const nutjs: nutjsTs = require('@nut-tree-fork/nut-js');
import nutjs from '@nut-tree-fork/nut-js';
console.log(moveMouseRel, moveMouseAbs);
let num = 0;
let timer;
timer = setInterval(() => {
  num += 1;
  if (num > 30) {
    clearInterval(timer);
    return;
  }
  // moveMouseAbs(100, 100);
  nutjs.mouse.move([{ x: 100, y: 100 }]);
}, 3000);
