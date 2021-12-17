#include <emscripten.h>
#include <emscripten/bind.h>

#include <stdio.h>
#include <errno.h>

#include <sys/socket.h>

#include "ssh2-session.h"

extern "C" {

static std::map<int, SESSION*> sockets;

extern void waitxxx();

int waitsocket(int fd)
{
#if 0
	int x = 0;

	x = EM_ASM_INIT({
		console.log($0);
		
		function delay(t, v) {
			return new Promise(function(resolve) {
				setTimeout(function() {
						resolve.bind(null, v);
						console.log('xxx')
				}, t)
			});
		}

		let nn = delay(1000, 1);

		console.log(nn);
	}, fd);

	fprintf(stderr, "x = %d\n", x);
#endif
	return -1;
}

/* 
 * socket() USE emscripten FS socket API
 */

int closesocket(int fd) 
{
	auto c = sockets.find(fd);
	if(c != sockets.end()) {
		// TODO: clean
		sockets.erase(fd);
	}

	return close(fd);
}

int connect(int fd, const struct sockaddr *addr, socklen_t addrlen) 
{
	auto c = sockets.find(fd);
	if(c != sockets.end()) {
		fprintf(stderr, "fd %d has exist!\n", fd);
		return -1;
	}

	sockets[fd] = (SESSION*)addr;

	return 0;
}

ssize_t recv(int fd, void *buffer, size_t length, int flags)
{
	auto c = sockets.find(fd);
	if(c == sockets.end()) {
		fprintf(stderr, "fd %d not found!\n", fd);
		return -1;
	}

	return sockets[fd]->recv(buffer, length, flags);
}

ssize_t recvfrom(int fd, void *buffer, size_t length, int flags, 
			struct sockaddr *src_addr, socklen_t *addrlen)
{
	return recv(fd, buffer, length, flags);
}

ssize_t send(int fd, const void *buffer, size_t length, int flags) 
{
	auto c = sockets.find(fd);
	if(c == sockets.end()) {
		fprintf(stderr, "fd %d not found!\n", fd);
		return -1;
	}

	emscripten::val vv(emscripten::typed_memory_view(length, (uint8_t*)buffer));
	sockets[fd]->send(vv, length);
	return length;
}

} /* ~ extern "C" */
