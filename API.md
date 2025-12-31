# libssh2.js API Documentation

Complete API reference for libssh2.js

## Table of Contents

- [Initialization](#initialization)
- [Session](#session)
- [Channel](#channel)
- [SFTP](#sftp)
- [Constants](#constants)
- [Error Handling](#error-handling)

## Initialization

### `init(wasmModule)`

Initialize the libssh2 library.

**Parameters:**
- `wasmModule` (Object): The compiled WASM module

**Returns:** `Promise<LibSSH2>` - Initialized library instance

**Example:**
```javascript
const ssh2 = await init(wasmModule);
```

---

## Session

### `createSession(socket, options)`

Create a new SSH session.

**Parameters:**
- `socket` (WebSocket | Socket): WebSocket or Node.js Socket object
- `options` (Object, optional):
  - `onerror` (Function): Error callback `(err: number, msg: string) => void`
  - `onclose` (Function): Close callback `(err: number, socket: any) => void`

**Returns:** `Session` - SSH session object

**Example:**
```javascript
const session = ssh2.createSession(ws, {
  onerror: (err, msg) => console.error(msg),
  onclose: () => console.log('Closed')
});
```

### `session.login(username, password)`

Authenticate with username and password.

**Parameters:**
- `username` (string): Username
- `password` (string): Password

**Returns:** `Promise<void>`

**Example:**
```javascript
await session.login('user', 'password');
```

### `session.CHANNEL()`

Create a new command channel.

**Returns:** `Promise<Channel>`

**Example:**
```javascript
const channel = await session.CHANNEL();
```

### `session.SFTP()`

Create SFTP subsystem.

**Returns:** `Promise<SFTP>`

**Example:**
```javascript
const sftp = await session.SFTP();
```

### `session.TCPIP(host, port)`

Create TCP port forwarding channel.

**Parameters:**
- `host` (string): Remote host
- `port` (number): Remote port

**Returns:** `Promise<Channel>`

**Example:**
```javascript
const tcpChannel = await session.TCPIP('localhost', 3000);
```

### `session.fingerprint()`

Get server's host key fingerprint.

**Returns:** `string` - SHA1 fingerprint in hex format

**Example:**
```javascript
const fp = session.fingerprint();
console.log('Fingerprint:', fp);
```

### `session.close()`

Close the SSH session and underlying socket.

**Returns:** `void`

---

## Channel

### `channel.exec(command)`

Execute a command on the remote server.

**Parameters:**
- `command` (string): Command to execute

**Returns:** `Promise<void>`

**Example:**
```javascript
await channel.exec('ls -la');
const output = await channel.read();
```

### `channel.shell()`

Start an interactive shell.

**Returns:** `Promise<void>`

**Example:**
```javascript
await channel.shell();
channel.onmessage = (err, data) => console.log(data);
await channel.write('ls\n');
```

### `channel.read()`

Read output from channel.

**Returns:** `Promise<string>` - Channel output

**Example:**
```javascript
const output = await channel.read();
```

### `channel.read_err()`

Read error output (stderr) from channel.

**Returns:** `Promise<string>` - Error output

**Example:**
```javascript
const errors = await channel.read_err();
```

### `channel.write(data)`

Write data to channel.

**Parameters:**
- `data` (string): Data to write

**Returns:** `Promise<number>` - Bytes written

**Example:**
```javascript
await channel.write('echo "Hello"\n');
```

### `channel.pty(term)`

Request a pseudo-terminal.

**Parameters:**
- `term` (string): Terminal type (e.g., 'xterm', 'vt100')

**Returns:** `Promise<void>`

**Example:**
```javascript
await channel.pty('xterm-256color');
```

### `channel.pty_size(width, height)`

Set terminal size.

**Parameters:**
- `width` (number): Terminal width in characters
- `height` (number): Terminal height in characters

**Returns:** `Promise<void>`

**Example:**
```javascript
await channel.pty_size(80, 24);
```

### `channel.setenv(name, value)`

Set environment variable.

**Parameters:**
- `name` (string): Variable name
- `value` (string): Variable value

**Returns:** `Promise<void>`

**Example:**
```javascript
await channel.setenv('LANG', 'en_US.UTF-8');
```

### `channel.eof()`

Check if channel has reached end-of-file.

**Returns:** `Promise<boolean>`

### `channel.flush()`

Flush the channel.

**Returns:** `Promise<void>`

### `channel.close()`

Close the channel.

**Returns:** `Promise<void>`

### Event Handlers

#### `channel.onmessage`

Called when data is received.

**Type:** `(err: number, data: string) => void`

**Example:**
```javascript
channel.onmessage = (err, data) => {
  console.log('Received:', data);
};
```

#### `channel.onerror`

Called on error.

**Type:** `(err: number, msg: string) => void`

#### `channel.onclose`

Called when channel closes.

**Type:** `() => void`

---

## SFTP

### `sftp.open(path, flags, mode)`

Open a file.

**Parameters:**
- `path` (string): File path
- `flags` (number): Open flags (see SFTP.FLAGS)
- `mode` (number): File permissions (octal)

**Returns:** `Promise<SFTPHandle>`

**Example:**
```javascript
const file = await sftp.open(
  '/remote/file.txt',
  ssh2.SFTP.FLAGS.FXF_READ,
  0
);
```

### `sftp.opendir(path)`

Open a directory.

**Parameters:**
- `path` (string): Directory path

**Returns:** `Promise<SFTPDirHandle>`

**Example:**
```javascript
const dir = await sftp.opendir('/remote/path');
```

### `sftp.stat(path)`

Get file/directory status.

**Parameters:**
- `path` (string): Path

**Returns:** `Promise<SFTPAttributes>`

**Example:**
```javascript
const attrs = await sftp.stat('/remote/file.txt');
console.log('Size:', attrs.filesize);
console.log('Permissions:', attrs.perm.toString(8));
```

### `sftp.lstat(path)`

Get file/directory status (don't follow symlinks).

**Parameters:**
- `path` (string): Path

**Returns:** `Promise<SFTPAttributes>`

### `sftp.mkdir(path, mode)`

Create directory.

**Parameters:**
- `path` (string): Directory path
- `mode` (number): Permissions (octal)

**Returns:** `Promise<void>`

**Example:**
```javascript
await sftp.mkdir('/remote/newdir', 0o755);
```

### `sftp.rmdir(path)`

Remove directory.

**Parameters:**
- `path` (string): Directory path

**Returns:** `Promise<void>`

### `sftp.unlink(path)`

Delete file.

**Parameters:**
- `path` (string): File path

**Returns:** `Promise<void>`

**Example:**
```javascript
await sftp.unlink('/remote/file.txt');
```

### `sftp.rename(source, dest, flags)`

Rename/move file.

**Parameters:**
- `source` (string): Source path
- `dest` (string): Destination path
- `flags` (number): Rename flags

**Returns:** `Promise<void>`

**Example:**
```javascript
await sftp.rename(
  '/old/path.txt',
  '/new/path.txt',
  ssh2.SFTP.RENAME_OVERWRITE
);
```

### `sftp.readlink(path)`

Read symbolic link.

**Parameters:**
- `path` (string): Symlink path

**Returns:** `Promise<string>` - Link target

### `sftp.symlink(orig, dest, type)`

Create symbolic link.

**Parameters:**
- `orig` (string): Original path
- `dest` (string): Link path
- `type` (number): Link type

**Returns:** `Promise<string>`

### `sftp.realpath(path)`

Resolve path to absolute path.

**Parameters:**
- `path` (string): Path

**Returns:** `Promise<string>` - Absolute path

### `sftp.statvfs(path)`

Get filesystem statistics.

**Parameters:**
- `path` (string): Path

**Returns:** `Promise<SFTPStatVFS>`

### `sftp.shutdown()`

Shutdown SFTP subsystem.

**Returns:** `Promise<void>`

---

## SFTP Handle

### `handle.read()`

Read from file.

**Returns:** `Promise<string>` - File data

**Example:**
```javascript
const data = await handle.read();
```

### `handle.write(buffer)`

Write to file.

**Parameters:**
- `buffer` (string | ArrayBuffer): Data to write

**Returns:** `Promise<number>` - Bytes written

**Example:**
```javascript
await handle.write('Hello, World!');
```

### `handle.seek(offset)`

Seek to position.

**Parameters:**
- `offset` (number): Byte offset

**Returns:** `Promise<void>`

### `handle.tell()`

Get current position.

**Returns:** `Promise<number>` - Current offset

### `handle.fstat()`

Get file attributes.

**Returns:** `Promise<SFTPAttributes>`

### `handle.close()`

Close file handle.

**Returns:** `Promise<void>`

---

## SFTP Directory Handle

### `dirHandle.readdir()`

Read directory entry.

**Returns:** `Promise<string>` - Filename or empty string if end

**Example:**
```javascript
let entry;
while ((entry = await dir.readdir())) {
  console.log('File:', entry);
}
```

### `dirHandle.close()`

Close directory handle.

**Returns:** `Promise<void>`

---

## Constants

### ERROR

Error codes for SSH operations.

```javascript
ssh2.ERROR.NONE                    // 0
ssh2.ERROR.AUTHENTICATION_FAILED   // -18
ssh2.ERROR.CHANNEL_UNKNOWN         // -23
ssh2.ERROR.EAGAIN                  // -37 (try again)
// ... see full list in types
```

### ERRMSG

Error messages mapping.

```javascript
ssh2.ERRMSG['-18']  // 'AUTHENTICATION_FAILED'
```

### SFTP.FLAGS

File open flags.

```javascript
ssh2.SFTP.FLAGS.FXF_READ      // Read
ssh2.SFTP.FLAGS.FXF_WRITE     // Write
ssh2.SFTP.FLAGS.FXF_APPEND    // Append
ssh2.SFTP.FLAGS.FXF_CREAT     // Create if not exists
ssh2.SFTP.FLAGS.FXF_TRUNC     // Truncate
ssh2.SFTP.FLAGS.FXF_EXCL      // Exclusive create
```

### SFTP.MODE

File permissions.

```javascript
ssh2.SFTP.MODE.S_IRUSR  // Owner read
ssh2.SFTP.MODE.S_IWUSR  // Owner write
ssh2.SFTP.MODE.S_IXUSR  // Owner execute
ssh2.SFTP.MODE.S_IRWXU  // Owner all (0700)
ssh2.SFTP.MODE.S_IRWXG  // Group all (0070)
ssh2.SFTP.MODE.S_IRWXO  // Other all (0007)
```

### SFTP.TYPE

File types.

```javascript
ssh2.SFTP.TYPE.REGULAR     // Regular file
ssh2.SFTP.TYPE.DIRECTORY   // Directory
ssh2.SFTP.TYPE.SYMLINK     // Symbolic link
```

---

## Error Handling

### Try-Catch Pattern

```javascript
try {
  await session.login(username, password);
} catch (error) {
  console.error('Login failed:', ssh2.ERRMSG[error]);
}
```

### Check Error Codes

```javascript
if (error === ssh2.ERROR.AUTHENTICATION_FAILED) {
  console.error('Invalid credentials');
} else if (error === ssh2.ERROR.EAGAIN) {
  // Retry operation
}
```

### Session Error Handler

```javascript
const session = ssh2.createSession(ws, {
  onerror: (err, msg) => {
    if (err === ssh2.ERROR.SOCKET_DISCONNECT) {
      console.error('Connection lost');
    }
  }
});
```

---

## Complete Examples

### Execute Command

```javascript
const ssh2 = await init(wasmModule);
const ws = new WebSocket('wss://server.com');
const session = ssh2.createSession(ws);

ws.onopen = async () => {
  await session.login('user', 'pass');
  const channel = await session.CHANNEL();
  await channel.exec('hostname');
  const output = await channel.read();
  console.log(output);
  await channel.close();
  session.close();
};
```

### Interactive Shell

```javascript
const channel = await session.CHANNEL();
await channel.pty('xterm-256color');
await channel.pty_size(80, 24);
await channel.shell();

channel.onmessage = (err, data) => {
  process.stdout.write(data);
};

process.stdin.on('data', async (data) => {
  await channel.write(data.toString());
});
```

### File Upload via SFTP

```javascript
const sftp = await session.SFTP();
const file = await sftp.open(
  '/remote/file.txt',
  ssh2.SFTP.FLAGS.FXF_WRITE | ssh2.SFTP.FLAGS.FXF_CREAT,
  0o644
);

await file.write('File content here');
await file.close();
```

### List Directory

```javascript
const sftp = await session.SFTP();
const dir = await sftp.opendir('/path');

let entry;
while ((entry = await dir.readdir())) {
  const attrs = await sftp.stat(`/path/${entry}`);
  console.log(entry, attrs.filesize);
}

await dir.close();
```

---

For more examples, see [USAGE.md](USAGE.md).
