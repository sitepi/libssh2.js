# libssh2.js
LibSSH2 over Stream(WebSocket | WebRTC | Socket) on browser | nodejs

## downloads
* zlib (https://www.zlib.net/zlib-1.2.11.tar.gz)
* mbedtls (https://github.com/ARMmbed/mbedtls/archive/refs/tags/v2.27.0.zip)
* libssh2 (https://www.libssh2.org/download/libssh2-1.10.0.tar.gz)

## build libs

	cd builds 
	unzip mbedtls-v2.27.0.zip 
	tar -zxf zlib-1.2.11.tar.gz 
	tar -zxf libssh2-1.10.0.tar.gz 
	
	export EMPORTS=`pwd`/ports 

### zlib
	cd zlib-1.2.11 
	emconfigure ./configure --static --prefix=${EMPORTS}
	emmake make -j4 
	emmake make install
	cd ..

### mbedtls
	cd mbedtls-2.27.0 
	mkdir build && cd build 
	
	emcmake cmake .. \
		-DMBEDTLS_FATAL_WARNINGS=OFF \
		-DENABLE_TESTING=OFF \
		-DENABLE_PROGRAMS=OFF \
		-DENABLE_ZLIB_SUPPORT=ON \
		-DZLIB_LIBRARY=${EMPORTS} \
		-DZLIB_INCLUDE_DIR=${EMPORTS}/include \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_INSTALL_PREFIX=${EMPORTS}
	emmake make -j4 
	emmake make install 
	cd ../../

### libssh2
	cd libssh2-1.10.0 
	mkdir build && cd build 
	
	emcmake cmake .. \
		-DBUILD_SHARED_LIBS=OFF \
		-DCRYPTO_BACKEND=mbedTLS \
		-DENABLE_ZLIB_COMPRESSION=ON \
		-DENABLE_DEBUG_LOGGING=OFF \
		-DCLEAR_MEMORY=ON \
		-DBUILD_EXAMPLES=OFF \
		-DBUILD_TESTING=OFF \
		-DZLIB_LIBRARY=${EMPORTS} \
		-DZLIB_INCLUDE_DIR=${EMPORTS}/include \
		-DMBEDTLS_INCLUDE_DIR=${EMPORTS}/include \
		-DMBEDTLS_LIBRARY=${EMPORTS} \
		-DMBEDX509_LIBRARY=${EMPORTS} \
		-DMBEDCRYPTO_LIBRARY=${EMPORTS} \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_INSTALL_PREFIX=${EMPORTS} 
	emmake make -j4 
	emmake make install 
	cd ../../

## libssh2.js build
	mkdir build && cd build
	emcmake cmake -DCMAKE_BUILD_TYPE=Release ..
	emmake make

## test
	websockify 8100 127.0.0.1:22 & 
	python3 -m http.server 

