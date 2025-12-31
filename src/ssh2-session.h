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

#ifndef _SSH2_SESSION_H_
#define _SSH2_SESSION_H_

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <sys/socket.h>

#include <string>
#include <queue>
#include <vector>
#include <cstdint>
#include <cstring>

#include <emscripten/bind.h>

#include "ssh2-common.h"
#include "ssh2-channel.h"
#include "ssh2-sftp.h"

class SESSION {
public:
	SESSION(emscripten::val v)
		: fd(-1)
		, session(nullptr)
		, has_opened(false)
		, has_logined(false)
		, error(0)
	{
		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd < 0) {
			error = 1;
			return;
		}

		if (connect(fd, reinterpret_cast<struct sockaddr*>(this), sizeof(*this)) != 0) {
			error = 1;
			return;
		}

		session = libssh2_session_init();
		if (session) {
			libssh2_session_set_blocking(session, 0);
		} else {
			error = 2;
		}
	}

	~SESSION() 
	{
		if (session) {
			libssh2_session_disconnect(session, "Normal Shutdown");
			libssh2_session_free(session);
			session = nullptr;
		}
		if (fd >= 0) {
			closesocket(fd);
			fd = -1;
		}
	}

	void pushdata(const std::string& data)
	{
		// Optimize: Use string_view to avoid potential copies
		if (data.empty()) return;
		
		const auto* p = reinterpret_cast<const uint8_t*>(data.data());
		const size_t len = data.length();

		// Batch insert is more efficient than individual pushes
		for (size_t i = 0; i < len; ++i) {
			incoming.push(p[i]);
		}

		if (error && (error != LIBSSH2_ERROR_EAGAIN)) {
			error = 3;
		} else if (!has_opened) {
			handshake();
		}
	}

	ssize_t recv(void *buffer, size_t length, int flags) 
	{
		// Optimize: Check empty first for early exit
		if (incoming.empty()) {
			errno = EAGAIN;
			return -EAGAIN;
		}

		uint8_t *p = static_cast<uint8_t*>(buffer);
		ssize_t c = 0;
		const size_t max_read = std::min(length, incoming.size());

		// Optimize: Use size for loop limit instead of checking empty each iteration
		for (size_t i = 0; i < max_read; ++i) {
			*p++ = incoming.front();
			incoming.pop();
			++c;
		}

		return c;
	}

	void clean() 
	{
		// Clear the queue efficiently
		std::queue<uint8_t> empty;
		std::swap(incoming, empty);
	}

public:
	emscripten::val send = emscripten::val::null();

	const std::string& getFingerprint() const noexcept { return fingerprint; }
	emscripten::val getSendCb() const { return send; }
	void setSendCb(emscripten::val cb) { send = cb; }

	CHANNEL channel()
	{
		LIBSSH2_CHANNEL* ch = nullptr;

		if (!has_logined) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
		} else if (has_opened) {
			ch = libssh2_channel_open_session(session);
			if (!ch) {
				error = libssh2_session_last_errno(session);
			}
		}

		return CHANNEL(session, ch);
	}

	CHANNEL tcpip(const std::string& ipaddr, int port)
	{
		LIBSSH2_CHANNEL* ch = nullptr;

		if (!has_logined) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
		} else if (has_opened) {
			ch = libssh2_channel_direct_tcpip(session, ipaddr.c_str(), port);
			if (!ch) {
				error = libssh2_session_last_errno(session);
			}
		}

		return CHANNEL(session, ch);
	}

	SFTP sftp()
	{
		LIBSSH2_SFTP* sf = nullptr;

		if (!has_logined) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
		} else if (has_opened) {
			sf = libssh2_sftp_init(session);
			if (!sf) {
				error = libssh2_session_last_errno(session);
			}
		}

		return SFTP(session, sf);
	}

	std::string userauth(const std::string& user)
	{
		std::string methods;

		if (has_opened) {
			// Check what authentication methods are available
			const char* m = libssh2_userauth_list(session, user.c_str(), 
			                                         static_cast<unsigned int>(user.length()));
			if (m) {
				methods = m;
			} else {
				error = libssh2_session_last_errno(session);
			}
		} else {
			error = LIBSSH2_ERROR_SOCKET_NONE;
		}
		return methods;
	}

	int login(const std::string& user, const std::string& passwd)
	{
		int rc = LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		if (has_opened) {
			// Authenticate via password
			rc = libssh2_userauth_password(session, user.c_str(), passwd.c_str());
			if (rc == 0) {
				has_logined = true;
			}
		}
		return rc;
	}

	int getError() const noexcept {
		return error;
	}

private:
	int handshake()
	{
		const int rc = libssh2_session_handshake(session, fd);

		if (rc == 0) {
			const char* rsakey = libssh2_hostkey_hash(session, 
			                                             LIBSSH2_HOSTKEY_HASH_SHA1);
			if (rsakey) {
				char buff[64];
				size_t n = 0;
				for (int i = 0; i < 20; ++i) {
					n += snprintf(buff + n, sizeof(buff) - n, "%02X:", 
					             static_cast<unsigned char>(rsakey[i]));
				}
				if (n > 0) {
					fingerprint.assign(buff, n - 1); // Remove trailing ':'
				}
				has_opened = true;
			}
		} else if (rc != LIBSSH2_ERROR_EAGAIN) {
			error = rc;
		}

		return rc;
	}

private:
	int fd = -1;
	LIBSSH2_SESSION* session = nullptr;
	bool has_opened = false;
	bool has_logined = false;
	int error = 0;

	std::queue<uint8_t> incoming;
	std::string fingerprint;

	// Reserved for future use
	emscripten::val onerror = emscripten::val::null();
	emscripten::val onclose = emscripten::val::null();
};

#endif /* ~_SSH2_SESSION_H_ */
