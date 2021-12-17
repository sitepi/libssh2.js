const net = require('net')


var factory = require('../dist/libssh2.js');
var SESSION;

factory().then((libssh2) => {
	SESSION = libssh2.SESSION;
	//CHANNEL = libssh2.CHANNEL;
	//SFTP    = libssh2.SFTP;

	libssh2.init(0);

	test();
});

async function test() 
{
	let socket = new net.Socket();
	//let client = new SESSION(socket);

//	client.send = function(buffer, length) {
//		//console.log('to raw', buffer.length)
//		socket.write(buffer);
//	}

	socket.connect(22, '127.0.0.1', function() {
		socket.on('error', (err)=> {
			console.error('socket error', err);
		});

		socket.on('data', (msg) => {
			//console.log('to ssh2', msg.length);
			client.pushdata(msg);
		});

		socket.on('close', () => {
			console.error('socket closed');
			delete client
		});
		client = new SESSION(socket)

		client.send = function(buffer, length) {
                        socket.write(buffer);
                }

        });
}
