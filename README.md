# libssh2.js
is a porting of libssh2 to webassembly for browsers or nodejs

it run on STREAMs. STREAMs is a WebSocket, WebRTC Datachannel, or nodejs net.Socket

## BASIC USAGE
	<script src="../dist/libssh2.js"></script>
	<script>
		let libssh2, session, channel;
		const user = 'root', passwd='yourpass';
		Module().then((wasm) => {
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

## SETUP
you need a websocket service to forwarding SSH message
	
like

	$ websockify 8100 127.0.0.1:22 & 
	
## BUILD
you can build it

SEE [BUILD.md](https://github.com/routerplus/libssh2.js/blob/main/BUILD.md)

