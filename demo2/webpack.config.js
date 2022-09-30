const path = require('path');
module.exports = {
  mode: 'production',
  entry: './index.js',
  output: {
    clean: true, //清空构建目录
    path: path.resolve(__dirname, './dist'), //构建目录
  },
  optimization: {
    usedExports: true,
    // sideEffects: false,
    sideEffects: true,
    // sideEffects: 'flag',
    minimize: true,
  },
};
