/*
 * Copyright (c) 2021 RouterPlus Networks
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <emscripten.h>
#include <emscripten/bind.h>

#include <stdio.h>
#include <errno.h>

#include <sys/socket.h>

#include "ssh2-session.h"

extern "C" {

static std::map<int, SESSION*> sockets;

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
		return -1;
	}

	sockets[fd] = (SESSION*)addr;

	return 0;
}

ssize_t recv(int fd, void *buffer, size_t length, int flags)
{
	auto c = sockets.find(fd);
	if(c == sockets.end()) {
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
		return -1;
	}

	emscripten::val vv(emscripten::typed_memory_view(length, (uint8_t*)buffer));
	sockets[fd]->send(vv, length);
	return length;
}

} /* ~ extern "C" */
