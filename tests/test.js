const net = require('net')

var factory = require('../dist/libssh2.js');
var libssh2;

factory().then((wasm) => {
	libssh2 = wasm;
	libssh2.init(0);
	test();
});

let user  = 'user'
let passwd= 'xxxxxx'

function test() 
{
	let client;
	let socket = new net.Socket();

	client = libssh2.createSESSION(socket, ()=> {
		
		client.login(user, passwd, (rc) => {
			if(rc !== libssh2.ERROR.NONE) {
				console.log(rc, libssh2.ERRMSG[rc]);
			}
			else {
				has_logined = true;
				console.log(user,'logined');
			}
		});
	});
	socket.connect(22, '127.0.0.1');
}
