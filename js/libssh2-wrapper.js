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

/**
 * Modern async helper to handle EAGAIN retries with backoff
 * @param {Function} operation - The operation to execute (returns result or error code)
 * @param {Object} options - Configuration options
 * @param {number} options.initialDelay - Initial retry delay in ms (default: 10)
 * @param {number} options.maxDelay - Maximum retry delay in ms (default: 100)
 * @param {number} options.timeout - Timeout in ms (default: 30000)
 * @param {number} options.backoffFactor - Backoff multiplier (default: 1.5)
 * @returns {Promise} Promise that resolves with operation result
 */
const retryOnEagain = async (operation, options = {}) => {
	const {
		initialDelay = 10,
		maxDelay = 100,
		timeout = 30000,
		backoffFactor = 1.5
	} = options;

	const startTime = Date.now();
	let delay = initialDelay;
	let abortController = null;

	const sleep = (ms) => new Promise(resolve => {
		const timeoutId = setTimeout(resolve, ms);
		if (abortController) {
			abortController.signal.addEventListener('abort', () => {
				clearTimeout(timeoutId);
				resolve();
			});
		}
	});

	while (true) {
		// Check timeout
		if (Date.now() - startTime >= timeout) {
			throw {
				code: ERROR.TIMEOUT,
				message: 'Operation timeout'
			};
		}

		// Execute operation
		const result = operation();
		
		// Handle different result types
		if (typeof result === 'object' && result !== null) {
			// Result is {code, message} or {code, data}
			const code = result.code ?? result.rc ?? result;
			
			if (code === ERROR.NONE || code === 0) {
				return result;
			} else if (code !== ERROR.EAGAIN) {
				throw result;
			}
		} else {
			// Result is just a code number
			if (result === ERROR.NONE || result === 0) {
				return { code: result, message: ERRMSG[result] };
			} else if (result !== ERROR.EAGAIN) {
				throw { code: result, message: ERRMSG[result] };
			}
		}

		// EAGAIN: wait before retry with exponential backoff
		await sleep(delay);
		delay = Math.min(delay * backoffFactor, maxDelay);
	}
};

/**
 * Simplified async helper for operations without complex retry logic
 * @param {Function} operation - The operation to execute
 * @param {Object} errorContext - Optional error context (e.g., handle object)
 * @returns {Promise} Promise that resolves with operation result
 */
const asyncOp = async (operation, errorContext = null) => {
	return retryOnEagain(() => {
		const result = operation();
		const code = errorContext ? errorContext.error : (result?.code ?? result);
		
		if (typeof result === 'object' && result !== null && !('code' in result)) {
			// Result is data, get error from context
			return {
				code: code,
				data: result,
				message: ERRMSG[code]
			};
		}
		
		return {
			code: code,
			data: result,
			message: ERRMSG[code]
		};
	});
};

/**
 * Convert callback-style or promise-style function
 * @param {Function} asyncFn - Async function to wrap
 * @returns {Function} Function that accepts optional callback
 */
const callbackOrPromise = (asyncFn) => {
	return function(_cb) {
		const isCallback = typeof _cb === 'function';
		
		if (isCallback) {
			// Callback style
			asyncFn()
				.then(result => _cb(result.code ?? 0, result.data ?? result.message))
				.catch(error => _cb(error.code ?? -1, error.message ?? 'Unknown error'));
		} else {
			// Promise style
			return asyncFn();
		}
	};
};

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
		S_IFMT:         0o170000,    /* type of file mask */
		S_IFIFO:        0o010000,    /* named pipe (fifo) */
		S_IFCHR:        0o020000,    /* character special */
		S_IFDIR:        0o040000,    /* directory */
		S_IFBLK:        0o060000,    /* block special */
		S_IFREG:        0o100000,    /* regular */
		S_IFLNK:        0o120000,    /* symbolic link */
		S_IFSOCK:       0o140000,    /* socket */

		/* File mode */
		/* Read, write, execute/search by owner */
		S_IRWXU:        0o000700,    /* RWX mask for owner */
		S_IRUSR:        0o000400,    /* R for owner */
		S_IWUSR:        0o000200,    /* W for owner */
		S_IXUSR:        0o000100,    /* X for owner */
		/* Read, write, execute/search by group */
		S_IRWXG:        0o000070,    /* RWX mask for group */
		S_IRGRP:        0o000040,    /* R for group */
		S_IWGRP:        0o000020,    /* W for group */
		S_IXGRP:        0o000010,    /* X for group */
		/* Read, write, execute/search by others */
		S_IRWXO:        0o000007,    /* RWX mask for other */
		S_IROTH:        0o000004,    /* R for other */
		S_IWOTH:        0o000002,    /* W for other */
		S_IXOTH:        0o000001     /* X for other */
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
	close = callbackOrPromise(async () => {
		return asyncOp(() => isdir ? h.closedir() : h.close());
	}),
	
	fsetstat = callbackOrPromise(async () => {
		return asyncOp(() => {
			const msg = h.fsetstat();
			return { code: h.error, data: msg };
		});
	}),
	
	fstat = callbackOrPromise(async () => {
		return asyncOp(() => {
			const msg = h.fstat();
			return { code: h.error, data: msg };
		});
	}),
	
	fstatvfs = callbackOrPromise(async () => {
		return asyncOp(() => {
			const msg = h.fstatvfs();
			return { code: h.error, data: msg };
		});
	}),
	
	fsync = callbackOrPromise(async () => {
		return asyncOp(() => h.fsync());
	}),
	
	read = callbackOrPromise(async () => {
		return asyncOp(() => {
			const msg = h.read();
			return { code: 0, data: msg };
		});
	}),
	
	readdir = callbackOrPromise(async () => {
		return asyncOp(() => {
			const msg = h.readdir();
			return { code: 0, data: msg };
		});
	}),
	
	rewind = callbackOrPromise(async () => {
		return asyncOp(() => h.rewind());
	}),
	
	seek = (offset, _cb) => callbackOrPromise(async () => {
		return asyncOp(() => h.seek(offset));
	})(_cb),
	
	seek64 = (offset, _cb) => callbackOrPromise(async () => {
		return asyncOp(() => h.seek64(offset));
	})(_cb),
	
	shutdown = callbackOrPromise(async () => {
		return asyncOp(() => h.shutdown());
	}),
	
	tell = callbackOrPromise(async () => {
		return asyncOp(() => h.tell());
	}),
	
	tell64 = callbackOrPromise(async () => {
		return asyncOp(() => h.tell64());
	}),
	
	write = (buffer, _cb) => callbackOrPromise(async () => {
		const n = h.write(buffer);
		return { code: 0, data: n, message: ERRMSG[0] };
	})(_cb)
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
	lstat = (path, _cb) => callbackOrPromise(async () => {
		return asyncOp(() => {
			const attrs = sf.lstat(path);
			return { code: sf.error, data: attrs };
		});
	})(_cb),
	mkdir = (path, mode, _cb) => callbackOrPromise(async () => {
		return asyncOp(() => sf.mkdir(path, mode));
	})(_cb),
	open = (path, flags, mode, _cb) => callbackOrPromise(async () => {
		const type = SFTP.OPENFILE;
		let h;
		
		return retryOnEagain(() => {
			if (!h || !h.active) {
				h = sf.open(path, flags, mode, type);
			}
			
			const rc = sf.error;
			if (h.active) {
				return { code: rc, data: sftp_handle(h) };
			} else if (rc !== ERROR.EAGAIN) {
				throw { code: rc, message: ERRMSG[rc] };
			}
			return { code: ERROR.EAGAIN };
		});
	})(_cb),
	opendir = (path, _cb) => callbackOrPromise(async () => {
		let h;
		
		return retryOnEagain(() => {
			if (!h || !h.active) {
				h = sf.opendir(path);
			}

			const rc = sf.error;
			if (h.active) {
				return { code: rc, data: sftp_handle(h, true) };
			} else if (rc !== ERROR.EAGAIN) {
				throw { code: rc, message: ERRMSG[rc] };
			}
			return { code: ERROR.EAGAIN };
		});
	})(_cb),
	readlink = (path, _cb) => callbackOrPromise(async () => {
		return asyncOp(() => {
			const msg = sf.readlink(path);
			const rc = sf.error;
			if (rc === ERROR.NONE) {
				return { code: rc, data: msg };
			} else if (rc !== ERROR.EAGAIN) {
				const err = (rc === ERROR.SFTP_PROTOCOL) ? ERRMSG[rc] : SFTP.STATMSG[rc];
				throw { code: rc, message: err };
			}
			return { code: ERROR.EAGAIN };
		});
	})(_cb),

	unlink = (path, _cb) => callbackOrPromise(async () => {
		return asyncOp(() => sf.unlink(path));
	})(_cb),
	realpath = (path, _cb) => callbackOrPromise(async () => {
		return asyncOp(() => {
			const msg = sf.realpath(path);
			return { code: sf.error, data: msg };
		});
	})(_cb),

	rename = (source, dest, flags, _cb) => callbackOrPromise(async () => {
		return asyncOp(() => sf.rename(source, dest, flags));
	})(_cb),
	rmdir = (path, _cb) => callbackOrPromise(async () => {
		return asyncOp(() => sf.rmdir(path));
	})(_cb),
	setstat = (path, _cb) => callbackOrPromise(async () => {
		return asyncOp(() => sf.setstat(path));
	})(_cb),
	shutdown = (_cb) => callbackOrPromise(async () => {
		return asyncOp(() => sf.shutdown());
	})(_cb),
	stat = (path, _cb) => callbackOrPromise(async () => {
		return asyncOp(() => {
			const msg = sf.stat(path, SFTP.STAT);
			return { code: sf.error, data: msg };
		});
	})(_cb),
	statvfs = (path, _cb) => callbackOrPromise(async () => {
		return asyncOp(() => {
			const msg = sf.statvfs(path);
			return { code: sf.error, data: msg };
		});
	})(_cb),
	symlink = (orig, dest, type, _cb) => callbackOrPromise(async () => {
		return asyncOp(() => {
			const msg = sf.symlink(orig, dest, type);
			return { code: sf.error, data: msg };
		});
	})(_cb);

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
	close = (_cb) => callbackOrPromise(async () => {
		if (!ch.active) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			onerror(rc, ERRMSG[rc]);
			throw { code: rc, message: ERRMSG[rc] };
		}
		
		return retryOnEagain(() => {
			const rc = ch.close();
			if (rc === ERROR.NONE) {
				ch.active = false;
				type = CHANNEL.UNKNOWN;
				onclose();
				return { code: rc, message: ERRMSG[rc] };
			} else if (rc !== ERROR.EAGAIN) {
				throw { code: rc, message: ERRMSG[rc] };
			}
			return { code: ERROR.EAGAIN };
		});
	})(_cb),
	eof = (_cb) => callbackOrPromise(async () => {
		if (!ch.active) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			onerror(rc, ERRMSG[rc]);
			throw { code: rc, message: ERRMSG[rc] };
		}
		if (type === CHANNEL.UNKNOWN) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		
		return asyncOp(() => ch.eof());
	})(_cb),
	exec = (cmd, _cb) => callbackOrPromise(async () => {
		if (!ch.active) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		if (type === CHANNEL.UNKNOWN) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		
		return asyncOp(() => ch.exec(cmd));
	})(_cb),
	flush = (_cb) => callbackOrPromise(async () => {
		if (!ch.active) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		if (type === CHANNEL.UNKNOWN) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		
		return asyncOp(() => ch.flush());
	})(_cb),
	read = (_cb) => callbackOrPromise(async () => {
		if (!ch.active) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		if (type === CHANNEL.UNKNOWN) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		
		const msg = ch.read();
		return { code: 0, data: msg };
	})(_cb),
	read_err = (_cb) => callbackOrPromise(async () => {
		if (!ch.active) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		if (type === CHANNEL.UNKNOWN) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		
		const msg = ch.read_err();
		return { code: 0, data: msg };
	})(_cb),
	write = (msg, _cb) => callbackOrPromise(async () => {
		if (!ch.active) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		if (type === CHANNEL.UNKNOWN) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		
		return retryOnEagain(() => {
			const rc = ch.write(msg);
			if (rc === msg.length) {
				return { code: rc, message: ERRMSG[rc] };
			} else if (rc !== ERROR.EAGAIN) {
				throw { code: rc, message: ERRMSG[rc] };
			}
			return { code: ERROR.EAGAIN };
		});
	})(_cb),
	write_err = (msg, _cb) => callbackOrPromise(async () => {
		if (!ch.active) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		if (type === CHANNEL.UNKNOWN) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		
		return asyncOp(() => ch.write_err(msg));
	})(_cb),
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
	},
	x11loop = chloop
	;

	if(istcp) {
		chloop();
	}

	const shell = (_cb) => callbackOrPromise(async () => {
		if (!ch.active) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		if (type !== CHANNEL.UNKNOWN) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}

		let has_pty = false, has_shell = false;

		return retryOnEagain(() => {
			let rc = ERROR.NONE;
			
			if (!has_pty) {
				rc = ch.pty("xterm");
				has_pty = (rc === ERROR.NONE);
			}

			if (has_pty && !has_shell) {
				rc = ch.shell();
				has_shell = (rc === ERROR.NONE);
			}
			
			if (rc !== ERROR.NONE && rc !== ERROR.EAGAIN) {
				throw { code: rc, message: ERRMSG[rc] };
			}

			if (has_pty && has_shell) {
				type = CHANNEL.SHELL;
				chloop();
				return { code: rc, message: ERRMSG[rc] };
			}
			
			return { code: ERROR.EAGAIN };
		}, { initialDelay: 200, maxDelay: 200 });
	})(_cb),
	x11 = (screen, _cb) => callbackOrPromise(async () => {
		if (!ch.active) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		if (type !== CHANNEL.UNKNOWN) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}

		let has_x11 = false, has_pty = false, has_shell = false;

		return retryOnEagain(() => {
			let rc = ERROR.NONE;
			
			if (!has_pty) {
				rc = ch.pty("xterm");
				has_pty = (rc === ERROR.NONE);
			}

			if (has_pty && !has_x11) {
				rc = ch.x11_req(screen);
				has_x11 = (rc === ERROR.NONE);
			}
			
			if (rc !== ERROR.NONE && rc !== ERROR.EAGAIN) {
				throw { code: rc, message: ERRMSG[rc] };
			}

			if (has_pty && has_x11 && !has_shell) {
				rc = ch.shell();
				has_shell = (rc === ERROR.NONE);
			}
			
			if (rc !== ERROR.NONE && rc !== ERROR.EAGAIN) {
				throw { code: rc, message: ERRMSG[rc] };
			}

			if (has_pty && has_x11 && has_shell) {
				type = CHANNEL.X11;
				x11loop();
				return { code: rc, message: ERRMSG[rc] };
			}
			
			return { code: ERROR.EAGAIN };
		}, { initialDelay: 200, maxDelay: 200 });
	})(_cb),
	pty_size = (width, height, _cb) => callbackOrPromise(async () => {
		if (!ch.active) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}
		if (type !== CHANNEL.SHELL) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}

		return asyncOp(() => ch.pty_size(width, height));
	})(_cb)
	;

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
		pty_size,
		type: () => {return type;},
		onmessage: (cb) => { onmessage = cb }
	};
};

const createSESSION = (socket, _cb) => {
	const cb = _cb || nocb;
	let onerror = cb, onclose = cb;

	var sess = new ssh2Loader._SESSION(socket);

	let has_logined = false;
	let has_opened = false;
	let has_cb = false;

	// Modern connection waiting with exponential backoff
	const waitForConnection = async () => {
		const maxRetries = 25;
		const checkInterval = 100;
		
		for (let i = 0; i < maxRetries; i++) {
			if (has_opened) {
				cb(0, 'OK');
				has_cb = true;
				return;
			}
			await new Promise(resolve => setTimeout(resolve, checkInterval));
		}
		
		if (!has_cb) {
			cb(-1, 'TIMEOUT');
			has_cb = true;
		}
	};
	waitForConnection();

	if(typeof(socket.binaryType) !== 'undefined') {
		socket.binaryType = 'arraybuffer';
		socket.onopen = function() {
			console.log('WebSocket opened');
			cb(0, 'opened');
			has_cb = true;
		}
		socket.onerror = function(e) {
			console.error('WebSocket error', e);
			onerror(-1, e);
			has_cb = true;
		}
		socket.onclose = function() {
			console.error('WebSocket closed');
			sess = null; // Clear reference instead of delete
			onclose(-1, socket);
			has_cb = true;
		}
		socket.onmessage = function(e)  {
			sess.pushdata(e.data);
			has_opened = true;
		}
		sess.send = (buffer) => {
			socket.send(buffer);
		}
	}
	else {
		socket.on('error', (err)=> {
			console.error('socket error', err);
			onerror(-1, err);
			has_cb = true;
		});
		socket.on('close', () => {
			console.error('socket closed');
			sess = null; // Clear reference instead of delete
			onclose(-1, socket);
			has_cb = true;
		});
		socket.on('data', (msg) => {
			sess.pushdata(msg);
			has_opened = true;
		});
		sess.send = (buffer) => {
			socket.write(buffer);
		}
	}

	const
	login = (user, passwd, _cb) => callbackOrPromise(async () => {
		return retryOnEagain(() => {
			const rc = sess.login(user, passwd);
			if (rc === ERROR.NONE) {
				has_logined = true;
				return { code: rc, message: ERRMSG[rc] };
			} else if (rc !== ERROR.EAGAIN) {
				throw { code: rc, message: ERRMSG[rc] };
			}
			return { code: ERROR.EAGAIN };
		}, { initialDelay: 200, maxDelay: 200 });
	})(_cb),

	close = () => {
		if(typeof(socket.close) !== 'undefined') {
			socket.close();
		}
		else {
			socket.end();
		}
	},
	
	createSFTP = (_cb) => callbackOrPromise(async () => {
		if (!has_logined) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}

		let sf;
		return retryOnEagain(() => {
			if (!sf || !sf.active) {
				sf = sess.sftp();
			}

			if (sf.active) {
				return { code: ERROR.NONE, data: sftp(sf) };
			}
			return { code: ERROR.EAGAIN };
		});
	})(_cb),
	
	createCHANNEL = (_cb) => callbackOrPromise(async () => {
		if (!has_logined) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}

		let ch;
		return retryOnEagain(() => {
			if (!ch || !ch.active) {
				ch = sess.channel();
			}
			
			if (ch.active) {
				return { code: 0, data: channel(ch) };
			}
			return { code: ERROR.EAGAIN };
		});
	})(_cb),
	createTCPIP = (ipaddr, port, _cb) => callbackOrPromise(async () => {
		if (!has_logined) {
			const rc = ERROR.AUTHENTICATION_FAILED;
			throw { code: rc, message: ERRMSG[rc] };
		}

		let ch;
		return retryOnEagain(() => {
			if (!ch || !ch.active) {
				ch = sess.tcpip(ipaddr, port);
			}

			const rc = sess.error;
			if (ch.active) {
				return { code: ERROR.NONE, data: channel(ch, true) };
			} else if (rc !== ERROR.EAGAIN) {
				throw { code: rc, message: ERRMSG[rc] };
			}
			return { code: ERROR.EAGAIN };
		});
	})(_cb)
	;

	return {
		'SFTP':    createSFTP,
		'CHANNEL': createCHANNEL,
		'TCPIP':   createTCPIP,
		login,
		close,
		fingerprint: () => {return sess.fingerprint}
	}
};

ssh2Loader['ERROR'] = ERROR;
ssh2Loader['ERRMSG'] = ERRMSG;
ssh2Loader['SFTP'] = SFTP;
ssh2Loader['CHANNEL'] = CHANNEL;
ssh2Loader['createSESSION'] = createSESSION;
