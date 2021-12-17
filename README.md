# libssh2.js
libssh2 over STREAMs on browser | nodejs


## build libs
	SEE [BUILD.md](https://github.com/routerplus/libssh2.js/blob/main/BUILD.md)

## libssh2.js build
	cd dist
	emcmake cmake -DCMAKE_BUILD_TYPE=Release ..
	emmake make
	cd ..

## test
	$ node 
	Welcome to Node.js v14.15.5.
	Type ".help" for more information.
	> require('./tests/test.js')
	> libssh2-1.10.0 loaded
	handshake ok
	Fingerprint: 


	or
	
	websockify 8100 127.0.0.1:22 & 
	python3 -m http.server
	visit http://host:8000/tests/test.html 

