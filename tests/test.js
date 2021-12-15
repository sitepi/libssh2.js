const net = require('net')

var factory = require('../build/libssh2.js');
var SESSION;

factory().then((libssh2) => {

	SESSION = libssh2.SESSION;

	test();
});

function test() 
{
	let socket = new net.Socket();
	let client = new SESSION(socket);

	client.send = function(buffer, length) {
		// console.log('buffer', buffer);
		console.log('to raw', buffer.length)
		socket.write(buffer);
	}

	socket.connect(22, '127.0.0.1', function() {
		socket.on('error', (err)=> {
		
		});

		socket.on('data', (msg) => {
			console.log('to ssh', msg, msg.length);
			client.pushdata(msg);

		});

		socket.on('close', () => {

		});
        });

	//delete client
}
