# libssh2.js Usage Guide

Complete guide for using libssh2.js in various JavaScript environments.

## Table of Contents

- [Installation](#installation)
- [Basic Usage](#basic-usage)
- [Browser Usage](#browser-usage)
- [React Usage](#react-usage)
- [Vue.js Usage](#vuejs-usage)
- [Node.js Usage](#nodejs-usage)
- [API Reference](#api-reference)

## Installation

```bash
npm install libssh2.js
# or
yarn add libssh2.js
```

## Basic Usage

### CommonJS (Node.js)

```javascript
const initSSH2 = require('libssh2.js');
const wasmModule = require('./path/to/libssh2.wasm.js');

(async () => {
  // Initialize the library
  const ssh2 = await initSSH2(wasmModule);
  
  // Create WebSocket connection
  const ws = new WebSocket('ws://localhost:8080');
  
  // Create SSH session
  const session = ssh2.createSession(ws, {
    onerror: (err, msg) => console.error('Error:', err, msg),
    onclose: () => console.log('Connection closed')
  });
  
  // Wait for connection
  ws.onopen = async () => {
    try {
      // Login
      await session.login('username', 'password');
      console.log('Fingerprint:', session.fingerprint());
      
      // Create channel and execute command
      const channel = await session.CHANNEL();
      await channel.exec('ls -la');
      
      const output = await channel.read();
      console.log('Output:', output);
      
      await channel.close();
      session.close();
    } catch (error) {
      console.error('SSH Error:', error);
    }
  };
})();
```

### ES6 Modules

```javascript
import initSSH2 from 'libssh2.js';
import wasmModule from './path/to/libssh2.wasm.js';

async function main() {
  const ssh2 = await initSSH2(wasmModule);
  
  const ws = new WebSocket('ws://localhost:8080');
  const session = ssh2.createSession(ws);
  
  ws.onopen = async () => {
    await session.login('user', 'pass');
    const channel = await session.CHANNEL();
    await channel.shell();
    
    // Interactive shell
    channel.onmessage = (err, data) => {
      console.log(data);
    };
    
    await channel.write('echo "Hello SSH"\n');
  };
}

main();
```

## Browser Usage

### Basic HTML + JavaScript

```html
<!DOCTYPE html>
<html>
<head>
  <title>libssh2.js Demo</title>
</head>
<body>
  <h1>SSH Terminal</h1>
  <pre id="terminal"></pre>
  <input type="text" id="command" placeholder="Enter command">
  <button id="execute">Execute</button>

  <script type="module">
    import initSSH2 from './libssh2.js';
    
    const terminal = document.getElementById('terminal');
    const commandInput = document.getElementById('command');
    const executeBtn = document.getElementById('execute');
    
    let session, channel;
    
    async function connect() {
      const ssh2 = await initSSH2(window.LibSSH2Module);
      const ws = new WebSocket('wss://your-ssh-proxy.com');
      
      session = ssh2.createSession(ws, {
        onerror: (err, msg) => {
          terminal.textContent += `\nError: ${msg}`;
        }
      });
      
      ws.onopen = async () => {
        await session.login('username', 'password');
        channel = await session.CHANNEL();
        await channel.shell();
        
        channel.onmessage = (err, data) => {
          terminal.textContent += data;
        };
      };
    }
    
    executeBtn.addEventListener('click', async () => {
      const cmd = commandInput.value;
      if (channel && cmd) {
        await channel.write(cmd + '\n');
        commandInput.value = '';
      }
    });
    
    connect();
  </script>
</body>
</html>
```

## React Usage

### React Hook for SSH Connection

```jsx
import React, { useState, useEffect, useRef } from 'react';
import initSSH2 from 'libssh2.js';

function useSSH2(wasmModule, wsUrl) {
  const [ssh2, setSSH2] = useState(null);
  const [session, setSession] = useState(null);
  const [connected, setConnected] = useState(false);
  const [error, setError] = useState(null);
  const wsRef = useRef(null);

  useEffect(() => {
    let mounted = true;

    async function init() {
      try {
        const lib = await initSSH2(wasmModule);
        if (mounted) setSSH2(lib);
      } catch (err) {
        if (mounted) setError(err);
      }
    }

    init();

    return () => {
      mounted = false;
      if (session) session.close();
      if (wsRef.current) wsRef.current.close();
    };
  }, [wasmModule]);

  const connect = async (username, password) => {
    if (!ssh2 || !wsUrl) return;

    try {
      const ws = new WebSocket(wsUrl);
      wsRef.current = ws;

      const sess = ssh2.createSession(ws, {
        onerror: (err, msg) => setError(msg),
        onclose: () => setConnected(false)
      });

      ws.onopen = async () => {
        try {
          await sess.login(username, password);
          setSession(sess);
          setConnected(true);
        } catch (err) {
          setError(err.message);
        }
      };
    } catch (err) {
      setError(err.message);
    }
  };

  const disconnect = () => {
    if (session) {
      session.close();
      setSession(null);
      setConnected(false);
    }
  };

  return {
    ssh2,
    session,
    connected,
    error,
    connect,
    disconnect
  };
}

// SSH Terminal Component
function SSHTerminal({ wasmModule, wsUrl }) {
  const { session, connected, error, connect, disconnect } = useSSH2(wasmModule, wsUrl);
  const [output, setOutput] = useState('');
  const [command, setCommand] = useState('');
  const [channel, setChannel] = useState(null);
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');

  const handleConnect = async () => {
    await connect(username, password);
  };

  const handleCreateChannel = async () => {
    if (!session || !connected) return;

    try {
      const ch = await session.CHANNEL();
      await ch.shell();

      ch.onmessage = (err, data) => {
        setOutput(prev => prev + data);
      };

      setChannel(ch);
    } catch (err) {
      console.error('Channel error:', err);
    }
  };

  const handleExecute = async () => {
    if (!channel || !command) return;

    try {
      await channel.write(command + '\n');
      setCommand('');
    } catch (err) {
      console.error('Execute error:', err);
    }
  };

  return (
    <div style={{ padding: '20px', fontFamily: 'monospace' }}>
      <h2>SSH Terminal</h2>
      
      {!connected ? (
        <div>
          <input
            type="text"
            placeholder="Username"
            value={username}
            onChange={(e) => setUsername(e.target.value)}
          />
          <input
            type="password"
            placeholder="Password"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
          />
          <button onClick={handleConnect}>Connect</button>
        </div>
      ) : (
        <div>
          <button onClick={disconnect}>Disconnect</button>
          {!channel && <button onClick={handleCreateChannel}>Open Shell</button>}
        </div>
      )}

      {error && <div style={{ color: 'red' }}>Error: {error}</div>}

      {channel && (
        <>
          <pre style={{
            background: '#000',
            color: '#0f0',
            padding: '10px',
            height: '400px',
            overflow: 'auto'
          }}>
            {output}
          </pre>
          <div>
            <input
              type="text"
              value={command}
              onChange={(e) => setCommand(e.target.value)}
              onKeyPress={(e) => e.key === 'Enter' && handleExecute()}
              placeholder="Enter command..."
              style={{ width: '80%' }}
            />
            <button onClick={handleExecute}>Execute</button>
          </div>
        </>
      )}
    </div>
  );
}

export default SSHTerminal;
```

### Simple React Component

```jsx
import React, { useState, useEffect } from 'react';
import initSSH2 from 'libssh2.js';

function SSHFileManager({ wasmModule, wsUrl }) {
  const [ssh2, setSSH2] = useState(null);
  const [files, setFiles] = useState([]);
  const [currentPath, setCurrentPath] = useState('/');

  useEffect(() => {
    initSSH2(wasmModule).then(setSSH2);
  }, [wasmModule]);

  const listFiles = async () => {
    if (!ssh2) return;

    const ws = new WebSocket(wsUrl);
    const session = ssh2.createSession(ws);

    ws.onopen = async () => {
      try {
        await session.login('user', 'pass');
        const sftp = await session.SFTP();
        
        const dir = await sftp.opendir(currentPath);
        const fileList = [];
        
        let entry;
        do {
          entry = await dir.readdir();
          if (entry) fileList.push(entry);
        } while (entry);
        
        setFiles(fileList);
        await dir.close();
        session.close();
      } catch (err) {
        console.error('SFTP error:', err);
      }
    };
  };

  return (
    <div>
      <h2>SFTP File Manager</h2>
      <input 
        value={currentPath}
        onChange={(e) => setCurrentPath(e.target.value)}
      />
      <button onClick={listFiles}>List Files</button>
      
      <ul>
        {files.map((file, i) => (
          <li key={i}>{file}</li>
        ))}
      </ul>
    </div>
  );
}

export default SSHFileManager;
```

## Vue.js Usage

### Vue 3 Composition API

```vue
<template>
  <div class="ssh-terminal">
    <h2>SSH Terminal</h2>
    
    <div v-if="!connected" class="login-form">
      <input v-model="username" placeholder="Username" />
      <input v-model="password" type="password" placeholder="Password" />
      <button @click="handleConnect">Connect</button>
    </div>
    
    <div v-else>
      <button @click="handleDisconnect">Disconnect</button>
      <button v-if="!channel" @click="createChannel">Open Shell</button>
    </div>
    
    <div v-if="error" class="error">{{ error }}</div>
    
    <div v-if="channel" class="terminal-container">
      <pre class="terminal">{{ output }}</pre>
      <div class="input-group">
        <input 
          v-model="command"
          @keyup.enter="executeCommand"
          placeholder="Enter command..."
        />
        <button @click="executeCommand">Execute</button>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue';
import initSSH2 from 'libssh2.js';

const props = defineProps({
  wasmModule: Object,
  wsUrl: String
});

const ssh2 = ref(null);
const session = ref(null);
const channel = ref(null);
const connected = ref(false);
const error = ref(null);
const output = ref('');
const command = ref('');
const username = ref('');
const password = ref('');
const ws = ref(null);

onMounted(async () => {
  try {
    ssh2.value = await initSSH2(props.wasmModule);
  } catch (err) {
    error.value = err.message;
  }
});

onUnmounted(() => {
  if (session.value) session.value.close();
  if (ws.value) ws.value.close();
});

const handleConnect = async () => {
  if (!ssh2.value || !props.wsUrl) return;

  try {
    ws.value = new WebSocket(props.wsUrl);
    
    session.value = ssh2.value.createSession(ws.value, {
      onerror: (err, msg) => {
        error.value = msg;
      },
      onclose: () => {
        connected.value = false;
      }
    });

    ws.value.onopen = async () => {
      try {
        await session.value.login(username.value, password.value);
        connected.value = true;
        error.value = null;
      } catch (err) {
        error.value = err.message;
      }
    };
  } catch (err) {
    error.value = err.message;
  }
};

const handleDisconnect = () => {
  if (session.value) {
    session.value.close();
    session.value = null;
    connected.value = false;
    channel.value = null;
  }
};

const createChannel = async () => {
  if (!session.value || !connected.value) return;

  try {
    const ch = await session.value.CHANNEL();
    await ch.shell();

    ch.onmessage = (err, data) => {
      output.value += data;
    };

    channel.value = ch;
  } catch (err) {
    error.value = err.message;
  }
};

const executeCommand = async () => {
  if (!channel.value || !command.value) return;

  try {
    await channel.value.write(command.value + '\n');
    command.value = '';
  } catch (err) {
    error.value = err.message;
  }
};
</script>

<style scoped>
.ssh-terminal {
  padding: 20px;
  font-family: monospace;
}

.login-form input {
  margin: 5px;
  padding: 8px;
}

.error {
  color: red;
  margin: 10px 0;
}

.terminal {
  background: #000;
  color: #0f0;
  padding: 10px;
  height: 400px;
  overflow: auto;
  white-space: pre-wrap;
}

.input-group {
  margin-top: 10px;
}

.input-group input {
  width: 80%;
  padding: 8px;
}

.input-group button {
  padding: 8px 16px;
}
</style>
```

### Vue 2 Options API

```vue
<template>
  <div class="ssh-terminal">
    <h2>SSH Terminal</h2>
    <div v-if="!connected">
      <input v-model="username" placeholder="Username" />
      <input v-model="password" type="password" placeholder="Password" />
      <button @click="connect">Connect</button>
    </div>
    <pre v-if="connected">{{ output }}</pre>
  </div>
</template>

<script>
import initSSH2 from 'libssh2.js';

export default {
  name: 'SSHTerminal',
  props: {
    wasmModule: Object,
    wsUrl: String
  },
  data() {
    return {
      ssh2: null,
      session: null,
      channel: null,
      connected: false,
      output: '',
      username: '',
      password: ''
    };
  },
  async mounted() {
    this.ssh2 = await initSSH2(this.wasmModule);
  },
  beforeDestroy() {
    if (this.session) this.session.close();
  },
  methods: {
    async connect() {
      const ws = new WebSocket(this.wsUrl);
      this.session = this.ssh2.createSession(ws);

      ws.onopen = async () => {
        await this.session.login(this.username, this.password);
        this.connected = true;
        
        this.channel = await this.session.CHANNEL();
        await this.channel.shell();
        
        this.channel.onmessage = (err, data) => {
          this.output += data;
        };
      };
    }
  }
};
</script>
```

## Node.js Usage

### With TCP Socket

```javascript
const net = require('net');
const initSSH2 = require('libssh2.js');
const wasmModule = require('./libssh2.wasm.js');

async function sshConnect() {
  const ssh2 = await initSSH2(wasmModule);
  
  // Create TCP connection to SSH server
  const socket = net.createConnection({
    host: 'example.com',
    port: 22
  });
  
  const session = ssh2.createSession(socket, {
    onerror: (err, msg) => console.error('Error:', msg),
    onclose: () => console.log('Connection closed')
  });
  
  socket.on('connect', async () => {
    try {
      await session.login('username', 'password');
      console.log('Logged in, fingerprint:', session.fingerprint());
      
      // Execute command
      const channel = await session.CHANNEL();
      await channel.exec('uptime');
      
      const output = await channel.read();
      console.log('Output:', output);
      
      await channel.close();
      session.close();
    } catch (error) {
      console.error('SSH Error:', error);
    }
  });
}

sshConnect();
```

### SFTP File Transfer

```javascript
const initSSH2 = require('libssh2.js');
const fs = require('fs');

async function sftpUpload(host, username, password, localPath, remotePath) {
  const ssh2 = await initSSH2(wasmModule);
  const ws = new WebSocket(`ws://${host}`);
  const session = ssh2.createSession(ws);
  
  ws.onopen = async () => {
    await session.login(username, password);
    
    const sftp = await session.SFTP();
    const file = await sftp.open(
      remotePath,
      ssh2.SFTP.FLAGS.FXF_WRITE | ssh2.SFTP.FLAGS.FXF_CREAT | ssh2.SFTP.FLAGS.FXF_TRUNC,
      0o644
    );
    
    const data = fs.readFileSync(localPath);
    await file.write(data);
    await file.close();
    
    console.log('File uploaded successfully');
    session.close();
  };
}
```

## API Reference

For complete API documentation, see [API.md](API.md).

### Quick Reference

- `createSession(socket, options)` - Create SSH session
- `session.login(username, password)` - Authenticate
- `session.CHANNEL()` - Create command channel
- `session.SFTP()` - Create SFTP subsystem
- `session.TCPIP(host, port)` - Create TCP forwarding
- `channel.exec(command)` - Execute command
- `channel.shell()` - Open interactive shell
- `sftp.open(path, flags, mode)` - Open file
- `sftp.opendir(path)` - Open directory

## Error Handling

```javascript
try {
  await session.login(username, password);
} catch (error) {
  if (error === ssh2.ERROR.AUTHENTICATION_FAILED) {
    console.error('Invalid credentials');
  } else if (error === ssh2.ERROR.SOCKET_TIMEOUT) {
    console.error('Connection timeout');
  } else {
    console.error('SSH Error:', ssh2.ERRMSG[error]);
  }
}
```

## License

MIT
