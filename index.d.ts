/**
 * Type definitions for libssh2.js
 * LibSSH2 Over Stream (WebSocket|WebRTC|Socket) in browsers or Node.js
 * 
 * @version 0.1.0
 */

declare module 'libssh2.js' {
  // Error codes
  export interface ERROR {
    readonly NONE: 0;
    readonly SOCKET_NONE: -1;
    readonly BANNER_RECV: -2;
    readonly BANNER_SEND: -3;
    readonly INVALID_MAC: -4;
    readonly KEX_FAILURE: -5;
    readonly ALLOC: -6;
    readonly SOCKET_SEND: -7;
    readonly KEY_EXCHANGE_FAILURE: -8;
    readonly TIMEOUT: -9;
    readonly HOSTKEY_INIT: -10;
    readonly HOSTKEY_SIGN: -11;
    readonly DECRYPT: -12;
    readonly SOCKET_DISCONNECT: -13;
    readonly PROTO: -14;
    readonly PASSWORD_EXPIRED: -15;
    readonly FILE: -16;
    readonly METHOD_NONE: -17;
    readonly AUTHENTICATION_FAILED: -18;
    readonly PUBLICKEY_UNRECOGNIZED: -18;
    readonly PUBLICKEY_UNVERIFIED: -19;
    readonly CHANNEL_OUTOFORDER: -20;
    readonly CHANNEL_FAILURE: -21;
    readonly CHANNEL_REQUEST_DENIED: -22;
    readonly CHANNEL_UNKNOWN: -23;
    readonly CHANNEL_WINDOW_EXCEEDED: -24;
    readonly CHANNEL_PACKET_EXCEEDED: -25;
    readonly CHANNEL_CLOSED: -26;
    readonly CHANNEL_EOF_SENT: -27;
    readonly SCP_PROTOCOL: -28;
    readonly ZLIB: -29;
    readonly SOCKET_TIMEOUT: -30;
    readonly SFTP_PROTOCOL: -31;
    readonly REQUEST_DENIED: -32;
    readonly METHOD_NOT_SUPPORTED: -33;
    readonly INVAL: -34;
    readonly INVALID_POLL_TYPE: -35;
    readonly PUBLICKEY_PROTOCOL: -36;
    readonly EAGAIN: -37;
    readonly BUFFER_TOO_SMALL: -38;
    readonly BAD_USE: -39;
    readonly COMPRESS: -40;
    readonly OUT_OF_BOUNDARY: -41;
    readonly AGENT_PROTOCOL: -42;
    readonly SOCKET_RECV: -43;
    readonly ENCRYPT: -44;
    readonly BAD_SOCKET: -45;
    readonly KNOWN_HOSTS: -46;
    readonly CHANNEL_WINDOW_FULL: -47;
    readonly KEYFILE_AUTH_FAILED: -48;
    readonly RANDGEN: -49;
  }

  export type ERRMSG = Record<string, string>;

  // SFTP constants and types
  export interface SFTPConstants {
    readonly OPENFILE: 0;
    readonly OPENDIR: 1;
    readonly RENAME_OVERWRITE: 0x00000001;
    readonly RENAME_ATOMIC: 0x00000002;
    readonly RENAME_NATIVE: 0x00000004;
    readonly STAT: 0;
    readonly LSTAT: 1;
    readonly SETSTAT: 2;
    readonly SYMLINK: 0;
    readonly READLINK: 1;
    readonly REALPATH: 2;
    readonly DEFAULT_MODE: -1;

    readonly ATTR: {
      readonly SIZE: 0x00000001;
      readonly UIDGID: 0x00000002;
      readonly PERMISSIONS: 0x00000004;
      readonly ACMODTIME: 0x00000008;
      readonly EXTENDED: 0x80000000;
    };

    readonly ST: {
      readonly RDONLY: 0x00000001;
      readonly NOSUID: 0x00000002;
    };

    readonly TYPE: {
      readonly REGULAR: 1;
      readonly DIRECTORY: 2;
      readonly SYMLINK: 3;
      readonly SPECIAL: 4;
      readonly UNKNOWN: 5;
      readonly SOCKET: 6;
      readonly CHAR_DEVICE: 7;
      readonly BLOCK_DEVICE: 8;
      readonly FIFO: 9;
    };

    readonly MODE: {
      readonly S_IFMT: number;
      readonly S_IFIFO: number;
      readonly S_IFCHR: number;
      readonly S_IFDIR: number;
      readonly S_IFBLK: number;
      readonly S_IFREG: number;
      readonly S_IFLNK: number;
      readonly S_IFSOCK: number;
      readonly S_IRWXU: number;
      readonly S_IRUSR: number;
      readonly S_IWUSR: number;
      readonly S_IXUSR: number;
      readonly S_IRWXG: number;
      readonly S_IRGRP: number;
      readonly S_IWGRP: number;
      readonly S_IXGRP: number;
      readonly S_IRWXO: number;
      readonly S_IROTH: number;
      readonly S_IWOTH: number;
      readonly S_IXOTH: number;
    };

    readonly FLAGS: {
      readonly FXF_READ: 0x00000001;
      readonly FXF_WRITE: 0x00000002;
      readonly FXF_APPEND: 0x00000004;
      readonly FXF_CREAT: 0x00000008;
      readonly FXF_TRUNC: 0x00000010;
      readonly FXF_EXCL: 0x00000020;
    };

    readonly STATUS: {
      readonly OK: 0;
      readonly EOF: 1;
      readonly NO_SUCH_FILE: 2;
      readonly PERMISSION_DENIED: 3;
      readonly FAILURE: 4;
      readonly BAD_MESSAGE: 5;
      readonly NO_CONNECTION: 6;
      readonly CONNECTION_LOST: 7;
      readonly OP_UNSUPPORTED: 8;
      readonly INVALID_HANDLE: 9;
      readonly NO_SUCH_PATH: 10;
      readonly FILE_ALREADY_EXISTS: 11;
      readonly WRITE_PROTECT: 12;
      readonly NO_MEDIA: 13;
      readonly NO_SPACE_ON_FILESYSTEM: 14;
      readonly QUOTA_EXCEEDED: 15;
      readonly UNKNOWN_PRINCIPAL: 16;
      readonly LOCK_CONFLICT: 17;
      readonly DIR_NOT_EMPTY: 18;
      readonly NOT_A_DIRECTORY: 19;
      readonly INVALID_FILENAME: 20;
      readonly LINK_LOOP: 21;
    };
  }

  // File attributes
  export interface SFTPAttributes {
    flags: number;
    filesize: number;
    uid: number;
    gid: number;
    perm: number;
    atime: number;
    mtime: number;
  }

  // Filesystem statistics
  export interface SFTPStatVFS {
    bsize: number;
    frsize: number;
    blocks: number;
    bfree: number;
    bavail: number;
    files: number;
    ffree: number;
    favail: number;
    fsid: number;
    flag: number;
    namemax: number;
  }

  // SFTP Handle
  export interface SFTPHandle {
    close(): Promise<void>;
    fsetstat(): Promise<SFTPAttributes>;
    fstat(): Promise<SFTPAttributes>;
    fstatvfs(): Promise<SFTPStatVFS>;
    fsync(): Promise<void>;
    read(): Promise<string>;
    rewind(): Promise<void>;
    seek(offset: number): Promise<void>;
    seek64(offset: number): Promise<void>;
    shutdown(): Promise<void>;
    tell(): Promise<number>;
    tell64(): Promise<number>;
    write(buffer: string | ArrayBuffer): Promise<number>;
  }

  // SFTP Directory Handle
  export interface SFTPDirHandle {
    close(): Promise<void>;
    readdir(): Promise<string>;
  }

  // SFTP Subsystem
  export interface SFTP {
    lstat(path: string): Promise<SFTPAttributes>;
    mkdir(path: string, mode: number): Promise<void>;
    open(path: string, flags: number, mode: number): Promise<SFTPHandle>;
    opendir(path: string): Promise<SFTPDirHandle>;
    readlink(path: string): Promise<string>;
    unlink(path: string): Promise<void>;
    realpath(path: string): Promise<string>;
    rename(source: string, dest: string, flags: number): Promise<void>;
    rmdir(path: string): Promise<void>;
    setstat(path: string): Promise<void>;
    shutdown(): Promise<void>;
    stat(path: string): Promise<SFTPAttributes>;
    statvfs(path: string): Promise<SFTPStatVFS>;
    symlink(orig: string, dest: string, type: number): Promise<string>;
  }

  // Channel
  export interface Channel {
    close(): Promise<void>;
    eof(): Promise<boolean>;
    exec(command: string): Promise<void>;
    flush(): Promise<void>;
    read(): Promise<string>;
    read_err(): Promise<string>;
    write(message: string): Promise<number>;
    write_err(message: string): Promise<number>;
    pty(term: string): Promise<void>;
    pty_size(width: number, height: number): Promise<void>;
    setenv(name: string, value: string): Promise<void>;
    shell(): Promise<void>;
    x11_req(screen: number): Promise<void>;
    
    // Event handlers
    onmessage?: (err: number, msg: string) => void;
    onerror?: (err: number, msg: string) => void;
    onclose?: () => void;
  }

  export interface CHANNELConstants {
    readonly UNKNOWN: 0;
    readonly SHELL: 1;
    readonly TCPIP: 2;
    readonly X11: 3;
  }

  // SSH Session
  export interface Session {
    login(username: string, password: string): Promise<void>;
    close(): void;
    SFTP(): Promise<SFTP>;
    CHANNEL(): Promise<Channel>;
    TCPIP(ipaddr: string, port: number): Promise<Channel>;
    fingerprint(): string;
  }

  // Main library interface
  export interface LibSSH2 {
    ERROR: ERROR;
    ERRMSG: ERRMSG;
    SFTP: SFTPConstants;
    CHANNEL: CHANNELConstants;
    createSession(socket: WebSocket | any, options?: {
      onerror?: (err: number, msg: string) => void;
      onclose?: (err: number, socket: any) => void;
    }): Session;
    createSSH(socket: WebSocket | any, options?: {
      onerror?: (err: number, msg: string) => void;
      onclose?: (err: number, socket: any) => void;
    }): Session;
    version(): string;
  }

  // Main initialization function
  export default function init(wasmModule: any): Promise<LibSSH2>;
  
  // Named exports
  export const ERROR: ERROR;
  export const SFTP: SFTPConstants;
  export const CHANNEL: CHANNELConstants;
}
