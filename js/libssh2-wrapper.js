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

const nocb = (rc, msg) => {
	console.log(rc, msg);
}

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

const CHANNEL = {
	UNKNOWN: 0,
	SHELL: 	 1,
	TCPIP:   2,
	X11:     3
};

const SFTP = {
	/* Flags for open_ex() */
	OPENFILE:           0,
	OPENDIR:            1,

	/* Flags for rename_ex() */
	RENAME_OVERWRITE:   0x00000001,
	RENAME_ATOMIC:      0x00000002,
	RENAME_NATIVE:      0x00000004,

	/* Flags for stat_ex() */
	STAT:               0,
	LSTAT:              1,
	SETSTAT:            2,

	/* Flags for symlink_ex() */
	SYMLINK:            0,
	READLINK:           1,
	REALPATH:           2,
	
	/* Flags for sftp_mkdir() */
	DEFAULT_MODE:      -1,

	/* SFTP attribute flag bits */
	ATTR_SIZE:              0x00000001,
	ATTR_UIDGID:            0x00000002,
	ATTR_PERMISSIONS:       0x00000004,
	ATTR_ACMODTIME:         0x00000008,
	ATTR_EXTENDED:          0x80000000,

	ATTR: {
		SIZE:              0x00000001,
		UIDGID:            0x00000002,
		PERMISSIONS:       0x00000004,
		ACMODTIME:         0x00000008,
		EXTENDED:          0x80000000
	},

	/* SFTP statvfs flag bits */
	ST_RDONLY:              0x00000001,
	ST_NOSUID:              0x00000002,

	ST: {
		RDONLY:              0x00000001,
		NOSUID:              0x00000002
	},

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
	},
	STATMSG: {
/* SFTP Status Codes (returned by libssh2_sftp_last_error() ) */
		0: 'OK', //                     0,//UL
		1: 'EOF', //                    1,//UL
		2: 'NO_SUCH_FILE', //           2,//UL
		3: 'PERMISSION_DENIED', //      3,//UL
		4: 'FAILURE', //                4,//UL
		5: 'BAD_MESSAGE', //            5,//UL
		6: 'NO_CONNECTION', //          6,//UL
		7: 'CONNECTION_LOST', //        7,//UL
		8: 'OP_UNSUPPORTED', //         8,//UL
		9: 'INVALID_HANDLE', //         9,//UL
		10: 'NO_SUCH_PATH', //           10,//UL
		11: 'FILE_ALREADY_EXISTS', //    11,//UL
		12: 'WRITE_PROTECT', //          12,//UL
		13: 'NO_MEDIA', //               13,//UL
		14: 'NO_SPACE_ON_FILESYSTEM', // 14,//UL
		15: 'QUOTA_EXCEEDED', //         15,//UL
		16: 'UNKNOWN_PRINCIPLE', //      16,//UL /* Initial mis-spelling */
		//16: 'UNKNOWN_PRINCIPAL', //      16,//UL
		17: 'LOCK_CONFlICT', //          17,//UL /* Initial mis-spelling */
		17: 'LOCK_CONFLICT', //          17,//UL
		18: 'DIR_NOT_EMPTY', //          18,//UL
		19: 'NOT_A_DIRECTORY', //        19,//UL
		20: 'INVALID_FILENAME', //       20,//UL
		21: 'LINK_LOOP', //              21,//UL
	}
};

const sftp_handle = function(_h, _isdir) {
	const h = _h;
	const isdir = _isdir || false;
	var attrs = {};
	var st    = {};

	const 
	close = function(_cb) {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc = (isdir) ? h.closedir() : h.close();
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	fsetstat = function(_cb) {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const msg = h.fsetstat();
			const rc  = h.error;
			if(rc == ERROR.NONE) {
				res(rc, msg);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	fstat = function(_cb) {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const msg = h.fstat();
			const rc  = h.error;
			if(rc == ERROR.NONE) {
				res(rc, msg);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	fstatvfs = function(_cb) {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const msg = h.fstatvfs();
			const rc  = h.error;
			if(rc == ERROR.NONE) {
				res(rc, msg);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	fsync = function(_cb) {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc = h.fsync();
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	read = function(_cb) {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const msg = h.read();
			const rc  = 0;
			if(rc == ERROR.NONE) {
				res(rc, msg);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	readdir = function(_cb) {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const msg = h.readdir();
			const rc  = 0;
			if(rc == ERROR.NONE) {
				res(rc, msg);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	rewind = function(_cb) {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc = h.rewind();
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	seek = function(offset, _cb) {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc = h.seek(offset);
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	seek64 = function(offset, _cb) {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc = h.seek64(offset);
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	shutdown = function(_cb) {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc = h.shutdown();
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	tell = function(_cb) {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc = h.tell();
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	tell64 = function(_cb) {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc = h.tell64();
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	write = function(buffer, _cb) {
		const cb = (typeof(_cb) === 'function') ? _cb : nocb;
		return new Promise((resolve, reject) => {
			const n = h.write(buffer);
			const rc = 0;
			cb(rc, ERRMSG[rc]);
			resolve(rc);
		});
	}
	;

	return (isdir) ?
	{
		close,
		readdir,
	} : {
		close,
		fsetstat,
		fstat,
		fstatvfs,
		fsync,
		read,
		rewind,
		seek,
		seek64,
		shutdown,
		tell,
		tell64,
		write
	};
};

const sftp = (_sf) => {
	const sf = _sf || { active: false };

	const
	lstat = (path, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const attrs = sf.lstat(path);
			const rc    = sf.error;
			if(rc == ERROR.NONE) {
				res(rc, attrs);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	mkdir = (path, mode, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc = sf.mkdir(path, mode);
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	open = (path, flags, mode, type, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;
		
		var h;
		const _async = () => {

			if(typeof(h) === 'undefined') {
				h = sf.open(path, flags, mode, type);
			}
			else if(!h.active) {
				h = sf.open(path, flags, mode, type);
			}
			
			const rc = sf.error;
			if(rc === ERROR.NONE) {
				;
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}

			if(h.active) {
				res(rc, sftp_handle(h));
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	opendir = (path, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;
		
		var h;
		const _async = () => {
			if(typeof(h) === 'undefined') {
				h = sf.opendir(path);
			}
			else if(!h.active) {
				h = sf.opendir(path);
			}

			const rc = sf.error;
			if(rc === ERROR.NONE) {
				;
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}

			if(h.active) {
				res(rc, sftp_handle(h, true));
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	readlink = (path, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const msg = sf.readlink(path);
			const rc  = sf.error;
			if(rc == ERROR.NONE) {
				res(rc, msg);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},

	unlink = (path, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc  = sf.unlink(path);
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	realpath = (path, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const msg = sf.realpath(path);
			const rc  = sf.error;
			if(rc == ERROR.NONE) {
				res(rc, msg);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},

	rename = (source, dest, flags, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc  = sf.rename(source, dest, flags);
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	rmdir = (path, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc  = sf.readlink(path);
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	setstat = (path, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc  = sf.setstat(path);
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	shutdown = (_cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc  = sf.shutdown();
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
				//TODO: set flag
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	stat = (path, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const msg = sf.stat(path, SFTP.STAT);
			const rc  = sf.error;
			if(rc == ERROR.NONE) {
				res(rc, msg);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	statvfs = (path, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const msg = sf.statvfs(path);
			const rc  = sf.error;
			if(rc == ERROR.NONE) {
				res(rc, msg);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	symlink = (orig, dest, type, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const msg = sf.symlink(orig, dest, type);
			const rc  = sf.error;
			if(rc == ERROR.NONE) {
				res(rc, msg);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
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

const channel = (_ch, _istcp) => {
	let ch   = _ch || {active: false};
	let istcp= _istcp|| false;
	let type = (istcp) ? CHANNEL.TCPIP : CHANNEL.UNKNOWN;

	const oncb = (err, msg)=> {
		console.log(err, msg);
	}
	var onmessage = oncb, onerror = oncb, onclose = oncb;
	
	const
	close = (_cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			if(!ch.active) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				onerror(RC, ERRmsg[rc]);
				return rej(rc, ERRMSG[rc]);
			}
			const rc  = ch.close();
			if(rc == ERROR.NONE) {
				ch.active = false;
				type = CHANNEL.UNKNOWN;

				onclose();
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	eof = (_cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			if(!ch.active) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				onerror(RC, ERRmsg[rc]);
				return rej(rc, ERRMSG[rc]);
			}
			else if(type === CHANNEL.UNKNOWN) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			const rc  = ch.eof();
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	exec = (cmd, _cb)=> {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			if(!ch.active) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			else if(type === CHANNEL.UNKNOWN) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			const rc  = ch.exec(cmd);
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	flush = (_cb)=> {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			if(!ch.active) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			else if(type === CHANNEL.UNKNOWN) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			const rc  = ch.flush();
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	read = (_cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			if(!ch.active) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			else if(type === CHANNEL.UNKNOWN) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			const msg = ch.read();
			res(0, msg);
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	read_err = (_cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			if(!ch.active) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			else if(type === CHANNEL.UNKNOWN) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			const msg  = ch.read_err();
			res(0, msg);
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	write = (msg, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			if(!ch.active) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			else if(type === CHANNEL.UNKNOWN) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			const rc  = ch.write(msg);
			if(rc === msg.length) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	write_err = (msg, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			if(!ch.active) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			else if(type === CHANNEL.UNKNOWN) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			const rc  = ch.write_err(msg);
			if(rc == ERROR.NONE) {
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },100)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	chloop = () => {
		if(!ch.active) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			onerror(rc, ERRMSG[rc]);
		}
		else if(type === CHANNEL.UNKNOWN) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			onerror(rc, ERRMSG[rc]);
			console.log('shell?, tcpip? x11?');
		}
		else {
			const msg = ch.read();
			if(msg.length > 0) {
				onmessage(0, msg);
			}
			setTimeout(chloop, 100);
		}
	}
	;

	const shell = (_cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		let has_pty = false, has_shell = false;

		const _async = () => {
			if(!ch.active) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}

			if(type !== CHANNEL.UNKNOWN) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}

			var rc = ERROR.NONE;
			if(!has_pty) {
				rc = ch.pty();
				has_pty = (rc === ERROR.NONE) ? true: false;
			}

			if(has_pty && !has_shell) {
				rc = ch.shell();
				has_shell = (rc === ERROR.NONE) ? true: false;
			}
			
			if((rc !== ERROR.NONE) && (rc !== ERROR.EAGAIN)) {
				return rej(rc, ERRMSG[rc])
			}

			if(has_pty && has_shell) {
				type = CHANNEL.SHELL;
				chloop();
				res(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },200)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	x11 = (screen, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		let has_pty = false, has_x11 = false;

		const _async = () => {
			if(!ch.active) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}

			if(type !== CHANNEL.UNKNOWN) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}

			var rc = ERROR.NONE;
			if(!has_pty) {
				rc = ch.pty();
				has_pty = (rc === ERROR.NONE) ? true: false;
			}

			if(has_pty && !has_x11) {
				rc = ch.x11_req(screen);
				has_x11 = (rc === ERROR.NONE) ? true: false;
			}
			
			if((rc !== ERROR.NONE) && (rc !== ERROR.EAGAIN)) {
				return rej(rc, ERRMSG[rc])
			}

			if(has_pty && has_shell) {
				type = CHANNEL.X11;
				chloop();
				res(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },200)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	};

	return {
		close,
		//eof,
		exec,
		flush,
		read,
		//read_err,
		send: write,
		write,
		//write_err,
		shell,
		x11,
		type: () => {return type;},
		onmessage: (cb) => { onmessage = cb }
	};
};

const createSESSION = (socket, _cb) => {
	const cb = (typeof(_cb) === 'function') ? _cb : nocb;
	var sess = new Module._SESSION(socket);

	let has_logined = false;

	if(typeof(socket.binaryType) !== 'undefined') {
		socket.binaryType = 'arraybuffer';
		socket.onopen = function() {
			console.log('WebSocket opened');
		}
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
	login = (user, passwd, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		const _async = () => {
			const rc = sess.login(user, passwd);
			if(rc == ERROR.NONE) {
				has_logined = true;
				res(rc, ERRMSG[rc]);
			}
			else if (rc !== ERROR.EAGAIN) {
				rej(rc, ERRMSG[rc]);
			}
			else {
				setTimeout(()=> { _async() },200)
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},

	close = () => {
		if(typeof(socket.close) !== 'undefined') {
			socket.close();
		}
		else {
			socket.end();
		}
	},
	
	createSFTP = (_cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		let sf;
		const _async = () => {
			if(!has_logined) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			else if(typeof(sf) === 'undefined') {
				sf = sess.sftp();
			}
			else if(!sf.active) {
				sf = sess.sftp();
			}

			if(sf.active) {
				const rc = ERROR.NONE;
				res(rc, sftp(sf));
			}
			else {
				setTimeout(() => { _async() }, 100);
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	
	createCHANNEL = (_cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		let ch;
		const _async = () => {
			if(!has_logined) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			else if(typeof(ch) === 'undefined') {
				ch = sess.channel();
			}
			else if(!ch.active) {
				ch = sess.channel();
			}
			
			if(ch.active) {
				const rc = 0;
				res(rc, channel(ch));
			}
			else {
				setTimeout(() => { _async()}, 100);
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	},
	createTCPIP = (ipaddr, port, _cb) => {
		const iscb = (typeof(_cb) === 'function');
		let res = _cb || nocb, rej = _cb || nocb;

		let ch;
		const _async = () => {
			if(!has_logined) {
				const rc = ERROR.AUTHENTICATION_FAILED;
				return rej(rc, ERRMSG[rc]);
			}
			else if(typeof(ch) === 'undefined') {
				ch = sess.tcpip(ipaddr, port);
			}
			else if(!ch.active) {
				ch = sess.tcpip(ipaddr, port);
			}
			
			if(ch.active) {
				const rc = 0;
				res(rc, channel(ch, true));
			}
			else {
				setTimeout(() => { _async()}, 100);
			}
		}

		return (iscb) ? _async() : new Promise((resolve, reject) => {
			res = resolve; rej = reject; _async();
		})
	}
	;

	if(typeof(cb) !== 'undefined') {
		setTimeout(()=> { cb();　},50);
	}
	return {
		'SFTP':    createSFTP,
		'CHANNEL': createCHANNEL,
		'TCPIP':   createTCPIP,
		login,
		close,
		fingerprint: () => {return sess.fingerprint}
	}
};

Module['ERROR'] = ERROR;
Module['ERRMSG'] = ERRMSG;
Module['SFTP'] = SFTP;
Module['CHANNEL'] = CHANNEL;
Module['createSESSION'] = createSESSION;
