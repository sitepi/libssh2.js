# libssh2.js
is a porting of libssh2 to webassembly for browsers or nodejs

it run on STREAMs. STREAMs is a WebSocket, WebRTC Datachannel, or nodejs net.Socket

a DEMO:
https://peer2.io/9117f812b1346056ee2710f6b4aa6aa23d54eee2d610a143ab034d674501a178


## BASIC USAGE
	<script src="../dist/libssh2.js"></script>
	<script>
		let libssh2, session, channel;
		const user = 'root', passwd='yourpass';
		ssh2Loader().then((wasm) => {
			libssh2 = wasm;
			libssh2.init(0);
			doconn();
		});

		function doconn() {
			const url = 'ws://websocket_to_ssh2/api_url';
			session = libssh2.createSESSION(new WebSocket(url), (rc, err)=> {
				if(rc === libssh2.ERROR.NONE) {
					session.login(user, passwd, (rc, msg) => {
						if(rc === libssh2.ERROR.NONE) {
							session.CHANNEL((rc, _ch) => {
								if(rc === libssh2.ERROR.NONE) {
									channel = _ch;
									doshell();
								}
							});
						}
					});
				}
			});
		}

		function doshell() {
			channel.onmessage((rc, msg) => {
				console.log('got message', msg);
			});
			channel.shell((rc, msg) => {
				console.log(rc, msg);
				if(rc === libssh2.ERROR.NONE) {
					console.log('got shell ok\r\n');
					// some COMMANDs
					channel.send('cat /proc/version\n');
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

