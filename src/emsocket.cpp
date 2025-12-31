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

#include <cstdio>
#include <cerrno>
#include <cstdint>

#include <sys/socket.h>
#include <unordered_map>

#include "ssh2-session.h"

extern "C" {

// Use unordered_map for better performance
static std::unordered_map<int, SESSION*> sockets;

/* 
 * Socket implementation using Emscripten socket-like API
 * Maps file descriptors to SESSION objects for custom I/O handling
 */

int closesocket(int fd) 
{
	// Optimize: Direct erase with key if exists
	auto erased = sockets.erase(fd);
	// Note: erased count is 0 if not found, 1 if found
	(void)erased; // Suppress unused variable warning
	return close(fd);
}

int connect(int fd, const struct sockaddr *addr, socklen_t addrlen) 
{
	// Check for null pointer
	if (!addr) {
		errno = EFAULT;
		return -1;
	}

	// Optimize: Use emplace for better performance (avoids redundant lookup)
	auto result = sockets.emplace(fd, reinterpret_cast<SESSION*>(const_cast<struct sockaddr*>(addr)));
	if (!result.second) {
		// emplace failed, fd already exists
		errno = EISCONN;
		return -1;
	}

	return 0;
}

ssize_t recv(int fd, void *buffer, size_t length, int flags)
{
	if (!buffer) {
		errno = EFAULT;
		return -1;
	}

	const auto it = sockets.find(fd);
	if (it == sockets.end()) {
		errno = EBADF;
		return -1;
	}

	return it->second->recv(buffer, length, flags);
}

ssize_t recvfrom(int fd, void *buffer, size_t length, int flags, 
                 struct sockaddr *src_addr, socklen_t *addrlen)
{
	// For this implementation, recvfrom behaves like recv
	return recv(fd, buffer, length, flags);
}

ssize_t send(int fd, const void *buffer, size_t length, int flags) 
{
	if (!buffer) {
		errno = EFAULT;
		return -1;
	}

	const auto it = sockets.find(fd);
	if (it == sockets.end()) {
		errno = EBADF;
		return -1;
	}

	// Optimize: Create view directly without intermediate variable
	it->second->send(
		emscripten::val(emscripten::typed_memory_view(length, 
		                static_cast<const uint8_t*>(buffer))),
		length
	);
	return static_cast<ssize_t>(length);
}

} /* ~ extern "C" */
