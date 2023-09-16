const ComponentsPlugin = require('unplugin-vue-components/rspack');

/**
 * @type {import('@rspack/cli').Configuration}
 */
module.exports = {
  context: __dirname,
  entry: {
    main: './src/main.tsx',
  },
  builtins: {
    html: [
      {
        template: './index.html',
      },
    ],
  },
  module: {
    rules: [
      {
        test: /\.svg$/,
        type: 'asset',
      },
      {
        test: /\.jpg$/,
        type: 'asset',
      },
    ],
  },
  // 已经是最小案例了，用了ComponentsPlugin后，import aaaJpg from './aaa.jpg' 就不生效
  // 注释ComponentsPlugin之后，就正常了
  plugins: [ComponentsPlugin()],
};
