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

#define BUFF_LEN 4096

class CHANNEL {
public:
	CHANNEL(emscripten::val v)
	{

	}

	CHANNEL(LIBSSH2_SESSION *sess, LIBSSH2_CHANNEL *ch) :
			session(sess),
			channel(ch),
			active(false)
	{
		if(channel) {
			fprintf(stderr, "CHANNEL ok\n");
			active = true;
		}
	}

	int close() 
	{
		int rc = LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		if(active) {
			rc = libssh2_channel_close(channel);
			if(!rc) {
				libssh2_channel_free(channel);
				channel = NULL;
				active = false;
			}
		}
		return rc;
	}

	int eof() 
	{
		if(active) {
			return libssh2_channel_eof(channel);
		}
		return LIBSSH2_ERROR_CHANNEL_UNKNOWN;
	}

	int exec(std::string cmd) 
	{
		if(active) {
			return libssh2_channel_exec(channel, cmd.c_str());
		}
		return LIBSSH2_ERROR_CHANNEL_UNKNOWN;
	}

	int flush() 
	{
		if(active) {
			return libssh2_channel_flush(channel);
		}
		return LIBSSH2_ERROR_CHANNEL_UNKNOWN;
	}

	std::string read()
	{
		int n = 0;
		if(active) {
			n = libssh2_channel_read(channel, buffer, BUFF_LEN);
			error = (n < 0) ? libssh2_session_last_errno(session) : 0;
		}
		else {
			error = LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		}
		return (n>0) ? std::string(buffer, n) : nodata;
	}

	std::string read_err()
	{
		ssize_t n = 0;
		if(active) {
			n=libssh2_channel_read_stderr(channel, buffer, BUFF_LEN);
			error = (n < 0) ? libssh2_session_last_errno(session) : 0;
		}
		else {
			error = LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		}
		return (n>0) ? std::string(buffer, n) : nodata;
	}

	int pty() 
	{
		int rc = LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		if(active) {
			rc = libssh2_channel_request_pty(channel, "xterm");
			if(!rc) {
				fprintf(stderr, "pty ok\n");
			}
		}

		return rc;
	}

	int pty_size(int width, int height) 
	{
		int rc = LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		if(active) {
			rc = libssh2_channel_request_pty_size(channel, 
					width, height);
			if(!rc) {
				fprintf(stderr, "pty size(%d, %d) ok\n", width, height);
			}
		}

		return rc;
	}

	int x11_req(int screen) 
	{
		int rc = LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		if(active) {
			rc = libssh2_channel_x11_req(channel, screen);
			if(!rc) {
				fprintf(stderr, "x11 ok\n");
			}
		}

		return rc;
	}

	int setenv(std::string name, std::string value)
	{
		if(active) {
			return libssh2_channel_setenv(channel, 
					name.c_str(), value.c_str());
		}
		
		return LIBSSH2_ERROR_CHANNEL_UNKNOWN;
	}

	int shell() 
	{
		int rc = LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		if(active) {
			rc = libssh2_channel_shell(channel);
			if(!rc) {
				fprintf(stderr, "shell ok\n");
			}
		}
		
		return rc;
	}

	int write(std::string cmd)
	{
		int rc = LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		if(active) {
			rc = libssh2_channel_write(channel, 
					cmd.c_str(),cmd.length());
		}

		return rc;
	}

	int write_err(std::string cmd)
	{
		int rc = LIBSSH2_ERROR_CHANNEL_UNKNOWN;
		if(active) {
			rc = libssh2_channel_write_stderr(channel,
					cmd.c_str(),  cmd.length());
		}
		return rc;
	}

	bool getActive() const {
		return active;
	}

	bool getError() const {
		return error;
	}

private:
	LIBSSH2_SESSION *session = NULL;
	LIBSSH2_CHANNEL *channel = NULL;

	char buffer[BUFF_LEN];
	std::string nodata;

	bool active = false;
	int error = 0;
};

#endif /* ~_SSH2_CHANNEL_H_ */
