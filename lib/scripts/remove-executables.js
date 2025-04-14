const fs = require('fs');
const path = require('path');
const os = require('os');

const executables = {
  windows: 'convert-win.exe',
  mac: 'convert-mac',
  linux: 'convert-linux'
};

const binDirectory = path.join(__dirname, 'bin');
const system = os.platform();

switch (system) {
  case 'win32':
    if (fs.existsSync(path.join(binDirectory, executables.mac))) {
      fs.unlinkSync(path.join(binDirectory, executables.mac));
    }
    if (fs.existsSync(path.join(binDirectory, executables.linux))) {
      fs.unlinkSync(path.join(binDirectory, executables.linux));
    }
    break;
  case 'darwin':
    if (fs.existsSync(path.join(binDirectory, executables.windows))) {
      fs.unlinkSync(path.join(binDirectory, executables.windows));
    }
    if (fs.existsSync(path.join(binDirectory, executables.linux))) {
      fs.unlinkSync(path.join(binDirectory, executables.linux));
    }
    break;
  case 'linux':
    if (fs.existsSync(path.join(binDirectory, executables.windows))) {
      fs.unlinkSync(path.join(binDirectory, executables.windows));
    }
    if (fs.existsSync(path.join(binDirectory, executables.mac))) {
      fs.unlinkSync(path.join(binDirectory, executables.mac));
    }
    break;
  default:
    console.log('Unsupported system');
    break;
}
