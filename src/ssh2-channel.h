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
		int rc = 0;
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
		int rc = 0;
		if(active) {
			rc = libssh2_channel_eof(channel);
		}

		return rc;
	}

	int exec(std::string cmd) 
	{
		int rc = 0;
		if(active) {
			rc = libssh2_channel_exec(channel, cmd.c_str());
		}

		return rc;
	}

	int flush() 
	{
		int rc = 0;
		if(active) {
			rc = libssh2_channel_flush(channel);
		}

		return rc;
	}

	std::string read()
	{
		int n = 0;
		if(active) {
			n = libssh2_channel_read(channel, buffer, BUFF_LEN);
		}

		return (n) ? std::string(buffer, n) : nodata;
	}

	std::string read_err()
	{
		ssize_t n = 0;
		if(active) {
			n=libssh2_channel_read_stderr(channel, buffer, BUFF_LEN);
		}
		return (n) ? std::string(buffer, n) : nodata;
	}

	int pty() 
	{
		int rc = 0;
		if(active) {
			rc = libssh2_channel_request_pty(channel, "xterm");

			if(rc == LIBSSH2_ERROR_EAGAIN) {
					;
			}
			else if (rc) {
				int err = libssh2_session_last_errno(session);
				fprintf(stderr, "pty failed: %d %d\n", rc, err);
			}
			else {
				fprintf(stderr, "pty ok\n");
			}
		}

		return rc;
	}

	int setenv(std::string name, std::string value)
	{
		int rc = 0;
		if(active) {
			rc = libssh2_channel_setenv(channel, 
					name.c_str(), value.c_str());
		}

		return rc;
	}

	int shell() 
	{
		int rc = 0;
		if(active) {
			rc = libssh2_channel_shell(channel);
			if (rc == LIBSSH2_ERROR_EAGAIN) {
				;
			}
			else if(rc) {
				int err = libssh2_session_last_errno(session);
				fprintf(stderr, "shell err: %d %d\n", rc, err);
			}
			else {
				fprintf(stderr, "shell ok\n");
			}
		}

		return rc;
	}

	int write(std::string cmd)
	{
		int rc = 0;
		if(active) {
			rc = libssh2_channel_write(channel, 
					cmd.c_str(),cmd.length());
		}

		return rc;
	}

	int write_err(std::string cmd)
	{
		int rc = 0;
		if(active) {
			rc = libssh2_channel_write_stderr(channel,
					cmd.c_str(),  cmd.length());
		}
		return rc;
	}

	bool getActive() const {
		return active;
	}

private:
	LIBSSH2_SESSION *session = NULL;
	LIBSSH2_CHANNEL *channel = NULL;

	bool active = false;

	char buffer[BUFF_LEN];
	std::string nodata;
};

#endif /* ~_SSH2_CHANNEL_H_ */
