/*
 * Copyright (c) 2021 RouterPlus Networks
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

const ERROR = {
	NONE:         0,
	SOCKET_NONE: -1,
	BANNER_RECV: -2,
	BANNER_SEND: -3,
	INVALID_MAC: -4,
	KEX_FAILURE: -5,
	ALLOC:       -6,
	SOCKET_SEND: -7,
	KEY_EXCHANGE_FAILURE: -8,
	TIMEOUT:              -9,
	HOSTKEY_INIT:        -10,
	HOSTKEY_SIGN:        -11,
	DECRYPT:             -12,
	SOCKET_DISCONNECT:   -13,
	PROTO:               -14,
	PASSWORD_EXPIRED:    -15,
	FILE:                -16,
	METHOD_NONE:         -17,
	AUTHENTICATION_FAILED:  -18,
	PUBLICKEY_UNRECOGNIZED: -18,
	PUBLICKEY_UNVERIFIED:   -19,
	CHANNEL_OUTOFORDER:     -20,
	CHANNEL_FAILURE:        -21,
	CHANNEL_REQUEST_DENIED: -22,
	CHANNEL_UNKNOWN:        -23,
	CHANNEL_WINDOW_EXCEEDED:-24,
	CHANNEL_PACKET_EXCEEDED:-25,
	CHANNEL_CLOSED:         -26,
	CHANNEL_EOF_SENT:       -27,
	SCP_PROTOCOL:           -28,
	ZLIB:                   -29,
	SOCKET_TIMEOUT:         -30,
	SFTP_PROTOCOL:          -31,
	REQUEST_DENIED:         -32,
	METHOD_NOT_SUPPORTED:   -33,
	INVAL:                  -34,
	INVALID_POLL_TYPE:      -35,
	PUBLICKEY_PROTOCOL:     -36,
	EAGAIN:                 -37,
	BUFFER_TOO_SMALL:       -38,
	BAD_USE:                -39,
	COMPRESS:               -40,
	OUT_OF_BOUNDARY:        -41,
	AGENT_PROTOCOL:         -42,
	SOCKET_RECV:            -43,
	ENCRYPT:                -44,
	BAD_SOCKET:             -45,
	KNOWN_HOSTS:            -46,
	CHANNEL_WINDOW_FULL:    -47,
	KEYFILE_AUTH_FAILED:    -48,
	RANDGEN:                -49
};

ERRMSG = {
	  '0': 'NONE',//:         0,
	 '-1': 'SOCKET_NONE',//: -1,
	 '-2': 'BANNER_RECV',//: -2,
	 '-3': 'BANNER_SEND',//: -3,
	 '-4': 'INVALID_MAC',//: -4,
	 '-5': 'KEX_FAILURE',//: -5,
	 '-6': 'ALLOC',//:       -6,
	 '-7': 'SOCKET_SEND',//: -7,
	 '-8': 'KEY_EXCHANGE_FAILURE',//: -8,
	 '-9': 'TIMEOUT',//:              -9,
	'-10': 'HOSTKEY_INIT',//:        -10,
	'-11': 'HOSTKEY_SIGN',//:        -11,
	'-12': 'DECRYPT',//:             -12,
	'-13': 'SOCKET_DISCONNECT',//:   -13,
	'-14': 'PROTO',//:               -14,
	'-15': 'PASSWORD_EXPIRED',//:    -15,
	'-16': 'FILE',//:                -16,
	'-17': 'METHOD_NONE',//:         -17,
	'-18': 'AUTHENTICATION_FAILED',//:  -18,
	//-18: 'PUBLICKEY_UNRECOGNIZED',//: -18,
	'-19': 'PUBLICKEY_UNVERIFIED',//:   -19,
	'-20': 'CHANNEL_OUTOFORDER',//:     -20,
	'-21': 'CHANNEL_FAILURE',//:        -21,
	'-22': 'CHANNEL_REQUEST_DENIED',//: -22,
	'-23': 'CHANNEL_UNKNOWN',//:        -23,
	'-24': 'CHANNEL_WINDOW_EXCEEDED',//:-24,
	'-25': 'CHANNEL_PACKET_EXCEEDED',//:-25,
	'-26': 'CHANNEL_CLOSED',//:         -26,
	'-27': 'CHANNEL_EOF_SENT',//:       -27,
	'-28': 'SCP_PROTOCOL',//:           -28,
	'-29': 'ZLIB',//:                   -29,
	'-30': 'SOCKET_TIMEOUT',//:         -30,
	'-31': 'SFTP_PROTOCOL',//:          -31,
	'-32': 'REQUEST_DENIED',//:         -32,
	'-33': 'METHOD_NOT_SUPPORTED',//:   -33,
	'-34': 'INVAL',//:                  -34,
	'-35': 'INVALID_POLL_TYPE',//:      -35,
	'-36': 'PUBLICKEY_PROTOCOL',//:     -36,
	'-37': 'EAGAIN',//:                 -37,
	'-38': 'BUFFER_TOO_SMALL',//:       -38,
	'-39': 'BAD_USE',//:                -39,
	'-40': 'COMPRESS',//:               -40,
	'-41': 'OUT_OF_BOUNDARY',//:        -41,
	'-42': 'AGENT_PROTOCOL',//:         -42,
	'-43': 'SOCKET_RECV',//:            -43,
	'-44': 'ENCRYPT',//:                -44,
	'-45': 'BAD_SOCKET',//:             -45,
	'-46': 'KNOWN_HOSTS',//:            -46,
	'-47': 'CHANNEL_WINDOW_FULL',//:    -47,
	'-48': 'KEYFILE_AUTH_FAILED',//:    -48,
	'-49': 'RANDGEN',//:                -49
};

const SFTP = {
	TYPE: {
		/* SFTP filetypes */
		REGULAR:           1,
		DIRECTORY:         2,
		SYMLINK:           3,
		SPECIAL:           4,
		UNKNOWN:           5,
		SOCKET:            6,
		CHAR_DEVICE:       7,
		BLOCK_DEVICE:      8,
		FIFO:              9
	},

	MODE: {
	/*
	 * Reproduce the POSIX file modes here for systems that are not POSIX
	 * compliant.
	 *
	 * These is used in "permissions" of "struct _LIBSSH2_SFTP_ATTRIBUTES"
	 */
	
		/* File type */
		S_IFMT:         0170000,     /* type of file mask */
		S_IFIFO:        0010000,     /* named pipe (fifo) */
		S_IFCHR:        0020000,     /* character special */
		S_IFDIR:        0040000,     /* directory */
		S_IFBLK:        0060000,     /* block special */
		S_IFREG:        0100000,     /* regular */
		S_IFLNK:        0120000,     /* symbolic link */
		S_IFSOCK:       0140000,     /* socket */

		/* File mode */
		/* Read, write, execute/search by owner */
		S_IRWXU:        0000700,     /* RWX mask for owner */
		S_IRUSR:        0000400,     /* R for owner */
		S_IWUSR:        0000200,     /* W for owner */
		S_IXUSR:        0000100,     /* X for owner */
		/* Read, write, execute/search by group */
		S_IRWXG:        0000070,     /* RWX mask for group */
		S_IRGRP:        0000040,     /* R for group */
		S_IWGRP:        0000020,     /* W for group */
		S_IXGRP:        0000010,     /* X for group */
		/* Read, write, execute/search by others */
		S_IRWXO:        0000007,     /* RWX mask for other */
		S_IROTH:        0000004,     /* R for other */
		S_IWOTH:        0000002,     /* W for other */
		S_IXOTH:        0000001     /* X for other */
	},

	FLAGS: {
/* SFTP File Transfer Flags -- (e.g. flags parameter to sftp_open())
 * Danger will robinson... APPEND doesn't have any effect on OpenSSH servers */
		FXF_READ:                        0x00000001,
		FXF_WRITE:                       0x00000002,
		FXF_APPEND:                      0x00000004,
		FXF_CREAT:                       0x00000008,
		FXF_TRUNC:                       0x00000010,
		FXF_EXCL:                        0x00000020
	},

	STATUS: {
/* SFTP Status Codes (returned by libssh2_sftp_last_error() ) */
		OK:                       0,//UL
		EOF:                      1,//UL
		NO_SUCH_FILE:             2,//UL
		PERMISSION_DENIED:        3,//UL
		FAILURE:                  4,//UL
		BAD_MESSAGE:              5,//UL
		NO_CONNECTION:            6,//UL
		CONNECTION_LOST:          7,//UL
		OP_UNSUPPORTED:           8,//UL
		INVALID_HANDLE:           9,//UL
		NO_SUCH_PATH:             10,//UL
		FILE_ALREADY_EXISTS:      11,//UL
		WRITE_PROTECT:            12,//UL
		NO_MEDIA:                 13,//UL
		NO_SPACE_ON_FILESYSTEM:   14,//UL
		QUOTA_EXCEEDED:           15,//UL
		UNKNOWN_PRINCIPLE:        16,//UL /* Initial mis-spelling */
		UNKNOWN_PRINCIPAL:        16,//UL
		LOCK_CONFlICT:            17,//UL /* Initial mis-spelling */
		LOCK_CONFLICT:            17,//UL
		DIR_NOT_EMPTY:            18,//UL
		NOT_A_DIRECTORY:          19,//UL
		INVALID_FILENAME:         20,//UL
		LINK_LOOP:                21,//UL
	}
};

const sftp_handle = function(handle) {
	const 
	close = function(cb) {

	},

	closedir = function(cb) {

	},
	fsetstat = function(cb) {

	},

	fstat = function(cb) {

	},
	fstatvfs = function(cb) {

	},
	fsync = function(cb) {

	},
	read = function(cb) {

	},
	readdir = function(cb) {

	},
	rewind = function(cb) {

	},
	seek = function(offset) {

	},
	seek64 = function(offset) {

	},
	shutdown = function(cb) {

	},
	tell = function(cb) {

	},
	tell64 = function(cb) {

	},
	write = function(buffer, cb) {

	}
	;

	return {
		close,
		closedir,
		fsetstat,
		fstat,
		fstatvfs,
		fsync,
		read,
		readdir,
		rewind,
		seek,
		seek64,
		shutdown,
		tell,
		tell64,
		write
	};
};

const sfp = (sf) => {
	const 
	lstat = (path, cb) => {
		return new Promise((resolve, reject) => {
			const attrs = sf.lstat(path);
			const rc    = sf.last_error;
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, attrs);
				if(typeof(cb) !== 'undefined') {
					cb(rc, attrs);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					lstat(path, cb);
				},1000);
			}
		});
	},
				
	mkdir = (path, mode, cb) => {
		return new Promise((resolve, reject) => {
			const rc  = sf.mkdir(path, mode);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					mkdir(path, cb);
				},1000);
			}
		});
	},

	open = (path, flags, mode, type, cb) => {
		return new Promise((resolve, reject) => {
			const rc  = sf.open(path, flags, mode, type);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					open(path, flags, mode, type, cb);
				},1000);
			}
		});
	},

	opendir = (path, cb) => {
		return new Promise((resolve, reject) => {
			const rc  = sf.opendir(path);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					opendir(path, cb);
				},1000);
			}
		});
	},

	readlink = (path, cb) => {
		return new Promise((resolve, reject) => {
			const rc  = sf.readlink(path);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					readlink(path, cb);
				},1000);
			}
		});
	},

	unlink = (path, cb) => {
		return new Promise((resolve, reject) => {
			const rc  = sf.readlink(path);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					readlink(path, cb);
				},1000);
			}
		});
	},

	realpath = (path, cb) => {
		return new Promise((resolve, reject) => {
			const rc  = sf.readlink(path);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					readlink(path, cb);
				},1000);
			}
		});
	},

	rename = (source, dest, flags, cb) => {
		return new Promise((resolve, reject) => {
			const rc  = sf.readlink(path);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					readlink(path, cb);
				},1000);
			}
		});
	},

	rmdir = (path, cb) => {
		return new Promise((resolve, reject) => {
			const rc  = sf.readlink(path);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					readlink(path, cb);
				},1000);
			}
		});
	},

	setstat = (path, cb) => {
		return new Promise((resolve, reject) => {
			const rc  = sf.readlink(path);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					readlink(path, cb);
				},1000);
			}
		});
	},

	shutdown = (cb) => {
		return new Promise((resolve, reject) => {
			const rc  = sf.readlink(path);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					readlink(path, cb);
				},1000);
			}
		});
	},

	stat = (path, cb) => {
		return new Promise((resolve, reject) => {
			const rc  = sf.readlink(path);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					readlink(path, cb);
				},1000);
			}
		});
	},

	statvfs = (path, cb) => {
		return new Promise((resolve, reject) => {
			const rc  = sf.readlink(path);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					readlink(path, cb);
				},1000);
			}
		});
	},

	symlink = (orig, dest, type, cb) => {
		return new Promise((resolve, reject) => {
			const rc  = sf.readlink(path);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					readlink(path, cb);
				},1000);
			}
		});
	};

	return {
		lstat,
		mkdir,
		open,
		opendir,
		readlink,
		unlink,
		realpath,
		rename,
		rmdir,
		setstat,
		shutdown,
		stat,
		statvfs,
		symlink
	};
};

const channel = (cb) => {
	let ch;
	let has_pty, has_shell;

	const obj = {
		close: () => {
			ch.close();
		},
		eof: () => {
			ch.eof();
		},
		exec: (cmd)=> {
			ch.exec(cmd);
		},
		flush: ()=> {
		
		},
		read: () => {

		},
		read_err: () => {

		},
		write: (msg) => {

		},
		write_err: () => {

		},
		onmessage: () => {
				
		}
	};

	return new Promise((resolve, reject) => {
		if(ch.active) {
			if(!has_pty) {
				const rc = ch.pty();
				if(rc === 0) {
					has_pty = true;
				}
				else if(rc !== ERROR.EAGAIN) {
					setTimeout(()=> {
						SHELL(cb);
					},100);
				}
				else {
					//reject();
					resolve(rc);
				}
			}

			if(!has_shell) {
				const rc = ch.shell();
				if(rc === 0) {
					has_shell = true;
				}
				else if(rc !== ERROR.EAGAIN) {
					setTimeout(()=> {
						SHELL(cb);
					},100);
				}
				else {
					//reject();
					resolve(rc);
				}
			}

			resolve(rc);
		}
		else {
			ch = sess.CHANNEL();
			setTimeout(()=> {
				SHELL(user, passwd, cb);
			},100);
		}
	});
};

const createSESSION = (socket, cb) => {	
	var sess = new Module.SESSION(socket);

	sess.send = function(buffer) {
		socket.write(buffer);
	}

	if(typeof(socket.binaryType) !== 'undefined') {
		socket.binaryType = 'arraybuffer';
		socket.onerror = function(e) {
			console.error('WebSocket error', e);
		}
		socket.onclose = function() {
			console.error('WebSocket closed');
			//sess.close();
			delete sess;
		}
		socket.onmessage = function(e)  {
			sess.pushdata(e.data);
		}
		sess.send = (buffer) => {
			socket.send(buffer);
		}
	}
	else {
		socket.on('error', (err)=> {
			console.error('socket error', err);
		});
		socket.on('close', () => {
			console.error('socket closed');
			delete sess;
		});
		socket.on('data', (msg) => {
			sess.pushdata(msg);
		});
		sess.send = (buffer) => {
			socket.write(buffer);
		}
	}

	const
	login = (user, passwd, cb) => {
		return new Promise((resolve, reject) => {
			const rc = sess.login(user, passwd);
			if(rc == 0) {
				resolve(rc);
				if(typeof(cb) !== 'undefined') {
					cb(rc);
				}

				console.log('Authentication by password succeeded.');
			}
			else if(rc !== ERROR.EAGAIN) {
				//reject(rc);
				resolve(rc, ERRMSG[rc]);
				if(typeof(cb) !== 'undefined') {
					cb(rc, ERRMSG[rc]);
				}
				console.log(ERRMSG[rc]);
			}
			else {
				setTimeout(()=> {
					login(user, passwd, cb);
				},1000);
			}
		});
	},

	close = () => {
		if(typeof(socket.close) !== 'undefined') {
			socket.close();
		}
		else {
			socket.end();
		}
	},
	
	createSFTP = () => {

	},
	
	createCHANNEL = () => {

	}
	;

	if(typeof(cb) !== 'undefined') {
		setTimeout(()=> {
			cb();
		},50);
	}
	return {
		SFTP: createSFTP,
		SHELL: createCHANNEL,
		login,
		close,
		fingerprint: () => {return sess.fingerprint}
	}
};

Module['ERROR'] = ERROR;
Module['ERRMSG'] = ERRMSG;
Module['createSESSION'] = createSESSION;
