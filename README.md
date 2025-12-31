# libssh2.js

🚀 **LibSSH2 Over Stream** - SSH2 client for browsers and Node.js using WebAssembly

[![npm version](https://badge.fury.io/js/libssh2.js.svg)](https://www.npmjs.com/package/libssh2.js)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A WebAssembly port of libssh2 that enables SSH2 connections over various transport streams (WebSocket, WebRTC DataChannel, or Node.js net.Socket) directly in the browser or Node.js environment.

## ✨ Features

- 🌐 **Universal**: Works in browsers and Node.js
- 🔒 **Secure**: Full SSH2 protocol support with authentication
- 📦 **Modern**: ES6+ modules, TypeScript definitions, async/await
- ⚡ **Fast**: Compiled to WebAssembly for native performance
- 🎯 **Easy to Use**: Simple, promise-based API
- 🔌 **Flexible**: Supports WebSocket, WebRTC, and TCP sockets
- 📁 **SFTP**: Full SFTP subsystem support
- 🎨 **Framework Ready**: Works with React, Vue.js, Angular, etc.

## 📦 Installation

```bash
npm install libssh2.js
# or
yarn add libssh2.js
```

## 🚀 Quick Start

### Browser (ES6 Modules)

```javascript
import initSSH2 from 'libssh2.js';

// Initialize library
const ssh2 = await initSSH2(LibSSH2Module);

// Connect via WebSocket
const ws = new WebSocket('wss://your-ssh-proxy.com');
const session = ssh2.createSession(ws);

ws.onopen = async () => {
  // Login
  await session.login('username', 'password');
  console.log('Fingerprint:', session.fingerprint());
  
  // Execute command
  const channel = await session.CHANNEL();
  await channel.exec('ls -la');
  const output = await channel.read();
  console.log(output);
  
  await channel.close();
  session.close();
};
```

### Node.js (CommonJS)

```javascript
const initSSH2 = require('libssh2.js');
const net = require('net');

(async () => {
  const ssh2 = await initSSH2(require('./libssh2.wasm.js'));
  
  const socket = net.createConnection({
    host: 'example.com',
    port: 22
  });
  
  const session = ssh2.createSession(socket);
  
  socket.on('connect', async () => {
    await session.login('user', 'password');
    
    const channel = await session.CHANNEL();
    await channel.exec('hostname');
    const output = await channel.read();
    console.log('Output:', output);
    
    await channel.close();
    session.close();
  });
})();
```

### Interactive Shell

```javascript
const channel = await session.CHANNEL();
await channel.pty('xterm-256color');
await channel.shell();

channel.onmessage = (err, data) => {
  console.log(data);
};

await channel.write('echo "Hello SSH"\n');
```

### SFTP File Transfer

```javascript
const sftp = await session.SFTP();

// Upload file
const file = await sftp.open(
  '/remote/file.txt',
  ssh2.SFTP.FLAGS.FXF_WRITE | ssh2.SFTP.FLAGS.FXF_CREAT,
  0o644
);
await file.write('File content');
await file.close();

// List directory
const dir = await sftp.opendir('/remote/path');
let entry;
while ((entry = await dir.readdir())) {
  console.log('File:', entry);
}
await dir.close();
```

## 🎨 Framework Integration

### React

```jsx
import { useState, useEffect } from 'react';
import initSSH2 from 'libssh2.js';

function SSHTerminal() {
  const [output, setOutput] = useState('');
  
  useEffect(() => {
    let session, channel;
    
    (async () => {
      const ssh2 = await initSSH2(LibSSH2Module);
      const ws = new WebSocket('wss://ssh-server.com');
      session = ssh2.createSession(ws);
      
      ws.onopen = async () => {
        await session.login('user', 'pass');
        channel = await session.CHANNEL();
        await channel.shell();
        
        channel.onmessage = (err, data) => {
          setOutput(prev => prev + data);
        };
      };
    })();
    
    return () => {
      if (channel) channel.close();
      if (session) session.close();
    };
  }, []);
  
  return <pre>{output}</pre>;
}
```

### Vue 3

```vue
<template>
  <pre>{{ output }}</pre>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue';
import initSSH2 from 'libssh2.js';

const output = ref('');
let session, channel;

onMounted(async () => {
  const ssh2 = await initSSH2(LibSSH2Module);
  const ws = new WebSocket('wss://ssh-server.com');
  session = ssh2.createSession(ws);
  
  ws.onopen = async () => {
    await session.login('user', 'pass');
    channel = await session.CHANNEL();
    await channel.shell();
    
    channel.onmessage = (err, data) => {
      output.value += data;
    };
  };
});

onUnmounted(() => {
  if (channel) channel.close();
  if (session) session.close();
});
</script>
```

## 📚 Documentation

- **[API Documentation](API.md)** - Complete API reference
- **[Usage Guide](USAGE.md)** - Detailed usage examples for all platforms
- **[TypeScript Definitions](index.d.ts)** - Full TypeScript support

## 🔧 API Overview

### Session Management

```javascript
const session = ssh2.createSession(socket, options);
await session.login(username, password);
const fingerprint = session.fingerprint();
session.close();
```

### Channel Operations

```javascript
const channel = await session.CHANNEL();
await channel.exec(command);
await channel.shell();
const output = await channel.read();
await channel.write(data);
await channel.close();
```

### SFTP Operations

```javascript
const sftp = await session.SFTP();
const file = await sftp.open(path, flags, mode);
const dir = await sftp.opendir(path);
const attrs = await sftp.stat(path);
await sftp.mkdir(path, mode);
await sftp.unlink(path);
```

## 🛠️ Building from Source

```bash
# Clone repository
git clone https://github.com/yourusername/libssh2.js.git
cd libssh2.js

# Build dependencies
cd deps
./build.sh  # or build.bat on Windows

# Build WebAssembly
mkdir build && cd build
emcmake cmake ..
emmake make

# The output will be in dist/
```

## 📋 Requirements

- **Browser**: Modern browser with WebAssembly support
- **Node.js**: v12.0.0 or higher
- **Build**: Emscripten SDK (for building from source)

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## 📄 License

MIT License - see [LICENSE](LICENSE) file for details

## 🙏 Acknowledgments

- [libssh2](https://www.libssh2.org/) - The SSH2 library
- [Emscripten](https://emscripten.org/) - The WebAssembly compiler

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/libssh2.js/issues)
- **Documentation**: [Full Documentation](USAGE.md)
- **Examples**: See [examples/](examples/) directory

---

Made with ❤️ by [Jie](https://github.com/yourusername)
			});
		}
	</script>
## EXAMPLES
A full shell example is [examples/xterm.html](https://github.com/routerplus/libssh2.js/blob/main/examples/xterm.html)

A direct tcpip example is [examples/tcpip.html](https://github.com/routerplus/libssh2.js/blob/main/examples/tcpip.html)

A x11 forwarding example is [examples/x11.html](https://github.com/routerplus/libssh2.js/blob/main/examples/x11.html), it success but miss x11 handler

## SETUP
you need a websocket service to forwarding SSH message
	
like

	$ websockify 7681 127.0.0.1:22 & 
	
## BUILD
you can build it yourself

SEE [BUILD.md](https://github.com/routerplus/libssh2.js/blob/main/BUILD.md)

