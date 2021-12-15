#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/threading.h>

#include <stdio.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <poll.h>

#include <thread>

#include <queue>

#include "ssh2-session.h"

extern "C" {

static std::map<int, SESSION*> clients;

/* 
 * socket() USE emscripten FS socket alloc
 */

int closesocket(int sockfd) 
{
	auto client = clients.find(sockfd);
	if(client != clients.end()) {
		// TODO: clean 
	}

	return close(sockfd);
}

int connect(int sockfd, const struct sockaddr *addr,
		socklen_t addrlen) 
{
	auto client = clients.find(sockfd);
	if(client != clients.end()) {
		fprintf(stderr, "socket occupied!\n");
		return -1;
	}

	clients[sockfd] = (SESSION*)addr;

	return 0;
}

ssize_t recv(int sockfd, void *buffer, size_t length, int flags)
{
	auto client = clients.find(sockfd);
	if(client == clients.end()) {
		fprintf(stderr, "no socket %d\n", sockfd);
		return -1;
	}

	return clients[sockfd]->recv(buffer, length, flags);
}

ssize_t send(int sockfd, const void *buffer, size_t length, int flags) 
{
	auto client = clients.find(sockfd);
	if(client == clients.end()) {
		fprintf(stderr, "no socket: %d\n", sockfd);
		return -1;
	}

	emscripten::val cb = clients[sockfd]->getSendCallback();
	emscripten::val vv(emscripten::typed_memory_view(length, (uint8_t*)buffer));
	cb(vv, length);
	return length;
}

} /* ~ extern "C" */
