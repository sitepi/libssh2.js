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

#ifndef _SSH2_CHANNEL_H_
#define _SSH2_CHANNEL_H_

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <emscripten/bind.h>
#include <cstring>
#include <cstdio>
#include <array>

#include "ssh2-common.h"

// Forward declaration
class CHANNEL;

// X11 callback data structure
struct X11Data {
	char shost[64] = {0};
	int sport = 0;
};

/*
 * Callback to initialize the X11 forwarding.
 * Save the channel to loop on it, save the X11 forwarded socket to send
 * and receive info from our X server.
 */
static void x11_callback(LIBSSH2_SESSION *session, LIBSSH2_CHANNEL *channel,
                         char *shost, int sport, void **abstract)
{
	if (abstract && *abstract) {
		X11Data* data = static_cast<X11Data*>(*abstract);
		snprintf(data->shost, sizeof(data->shost), "%s", shost);
		data->sport = sport;
		fprintf(stderr, "X11: shost=%s, sport=%d\r\n", shost, sport);
	}
}

class CHANNEL {
public:
	CHANNEL(emscripten::val v)
		: session(nullptr)
		, channel(nullptr)
		, active(false)
		, error(0)
	{
		// Empty binding constructor
	}

	CHANNEL(LIBSSH2_SESSION* sess, LIBSSH2_CHANNEL* ch)
		: session(sess)
		, channel(ch)
		, active(ch != nullptr)
		, error(0)
	{
		std::memset(buffer, 0, sizeof(buffer));
	}

	// Destructor to ensure proper cleanup
	~CHANNEL() {
		if (active && channel) {
			libssh2_channel_close(channel);
			libssh2_channel_free(channel);
		}
	}

	// Disable copy, enable move
	CHANNEL(const CHANNEL&) = delete;
	CHANNEL& operator=(const CHANNEL&) = delete;
	CHANNEL(CHANNEL&& other) noexcept
		: session(other.session)
		, channel(other.channel)
		, active(other.active)
		, error(other.error)
		, x11_data(other.x11_data)
	{
		std::memcpy(buffer, other.buffer, sizeof(buffer));
		other.channel = nullptr;
		other.active = false;
	}
	CHANNEL& operator=(CHANNEL&& other) noexcept {
		if (this != &other) {
			if (active && channel) {
				libssh2_channel_close(channel);
				libssh2_channel_free(channel);
			}
			session = other.session;
			channel = other.channel;
			active = other.active;
			error = other.error;
			x11_data = other.x11_data;
			std::memcpy(buffer, other.buffer, sizeof(buffer));
			other.channel = nullptr;
			other.active = false;
		}
		return *this;
	}

	int close() 
	{
		if (!active) {
			return LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		}

		const int rc = libssh2_channel_close(channel);
		if (rc == 0) {
			libssh2_channel_free(channel);
			channel = nullptr;
			active = false;
		}
		return rc;
	}

private:
	// Helper to check if channel is active
	inline bool checkActive() const noexcept {
		return active && channel != nullptr;
	}

	// Helper to set error and return result
	inline int setError(int rc) noexcept {
		error = (rc < 0) ? libssh2_session_last_errno(session) : 0;
		return rc;
	}

public:
	int eof() const
	{
		return checkActive() ? libssh2_channel_eof(channel) 
		                     : LIBSSH2_ERROR_CHANNEL_UNKNOWN;
	}

	int exec(const std::string& cmd) 
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		}
		return setError(libssh2_channel_exec(channel, cmd.c_str()));
	}

	int flush() 
	{
		return checkActive() ? libssh2_channel_flush(channel) 
		                     : LIBSSH2_ERROR_CHANNEL_UNKNOWN;
	}

	std::string read()
	{
		if (!checkActive()) {
			error = LIBSSH2_ERROR_CHANNEL_UNKNOWN;
			return "";
		}

		// Check if we have X11 forwarding data to return
		if (x11_data.sport != 0) {
			// Optimize: Calculate exact size needed
			std::string result;
			result.reserve(128); // Reserve enough for JSON
			const int n = snprintf(buffer, BUFF_LEN, 
			                      "{\"shost\":\"%s\",\"sport\":%d}", 
			                      x11_data.shost, x11_data.sport);
			x11_data.sport = 0; // Reset after reading
			error = 0;
			if (n > 0) {
				result.assign(buffer, static_cast<size_t>(n));
			}
			return result;
		}

		const ssize_t n = libssh2_channel_read(channel, buffer, BUFF_LEN);
		error = (n < 0) ? libssh2_session_last_errno(session) : 0;
		// Optimize: Avoid string construction if no data
		return (n > 0) ? std::string(buffer, static_cast<size_t>(n)) : std::string();
	}

	std::string read_err()
	{
		if (!checkActive()) {
			error = LIBSSH2_ERROR_CHANNEL_UNKNOWN;
			return std::string();
		}

		const ssize_t n = libssh2_channel_read_stderr(channel, buffer, BUFF_LEN);
		error = (n < 0) ? libssh2_session_last_errno(session) : 0;
		return (n > 0) ? std::string(buffer, static_cast<size_t>(n)) : "";
	}

	int pty(const std::string& term) 
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		}
		return libssh2_channel_request_pty(channel, term.c_str());
	}

	int pty_size(int width, int height) 
	{
		return checkActive() ? libssh2_channel_request_pty_size(channel, width, height)
		                     : LIBSSH2_ERROR_CHANNEL_UNKNOWN;
	}

	int x11_req(int screen) 
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		}

		// Set callback with our X11 data as abstract pointer
		libssh2_session_callback_set(session, LIBSSH2_CALLBACK_X11,
						        reinterpret_cast<void*>(x11_callback));

		return libssh2_channel_x11_req_ex(channel, 0, nullptr, nullptr, screen);
	}

	int setenv(const std::string& name, const std::string& value)
	{
		return checkActive() ? libssh2_channel_setenv(channel, name.c_str(), value.c_str())
		                     : LIBSSH2_ERROR_CHANNEL_UNKNOWN;
	}

	int shell() 
	{
		return checkActive() ? libssh2_channel_shell(channel)
		                     : LIBSSH2_ERROR_CHANNEL_UNKNOWN;
	}

	int write(const std::string& cmd)
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		}
		return libssh2_channel_write(channel, cmd.data(), cmd.length());
	}

	int write_err(const std::string& cmd)
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		}
		return libssh2_channel_write_stderr(channel, cmd.data(), cmd.length());
	}

	bool getActive() const noexcept {
		return active;
	}

	int getError() const noexcept {
		return error;
	}

private:
	LIBSSH2_SESSION* session = nullptr;
	LIBSSH2_CHANNEL* channel = nullptr;
	char buffer[BUFF_LEN] = {0};
	bool active = false;
	int error = 0;
	X11Data x11_data;
};

#endif /* ~_SSH2_CHANNEL_H_ */
