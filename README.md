# libssh2.js
LibSSH2 over Stream(WebSocket | WebRTC | Socket) on browser | nodejs


## build libs
	SEE deps/BUILD.md

## libssh2.js build
	cd dist
	emcmake cmake -DCMAKE_BUILD_TYPE=Release ..
	emmake make
	cd ..

## test
	node tests/test.js

	or
	
	websockify 8100 127.0.0.1:22 & 
	python3 -m http.server
	visit http://host:8000/tests/test.html 

