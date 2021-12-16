/*
 *
 */
#ifndef _SSH2_CHANNEL_H_
#define _SSH2_CHANNEL_H_

#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>

#include <emscripten.h>
#include <emscripten/bind.h>

class CHANNEL {
public:
	CHANNEL(emscripten::val handle)
	{
		channel = libssh2_channel_open_session(session);
		if(!channel) {
			fprintf(stderr, "Unable to open a session\n");
		}
		else {
			fprintf(stderr, "channel ok\n");
		}
	}

	CHANNEL(LIBSSH2_SESSION *sess, LIBSSH2_CHANNEL *ch) :
		session(sess),
		channel(ch)
	{
		if(channel) {
			fprintf(stderr, "CHANNEL ok\n");
		}
	}
#if 0
	int get_exit_signal() {
		return libssh2_channel_get_exit_signal(channel, NULL, NULL, NULL, NULL, NULL, NULL);
	}

	int get_exit_status() {
		return libssh2_channel_get_exit_status(channel);
	}
#endif
    	int close() {
		return libssh2_channel_close(channel);
	}

	int eof() {
		return libssh2_channel_eof(channel);
	}

	int exec(std::string cmd) {
		return libssh2_channel_exec(channel, cmd.c_str());
	}

	int flush() {
		return libssh2_channel_flush(channel);
	}

	std::string read()
	{
                char buf[1024];
                int len = libssh2_channel_read(channel, buf, 1024);

                if(len) {
                        std::string data(buf, len);
                        return data;
                }
                else {
                        std::string data(buf, 1);
                        return data;
                }
        }

	std::string read_stderr()
	{
		char buf[1024];
		int len = libssh2_channel_read_stderr(channel, buf, 1024);

		if(len) {
			std::string data(buf, len);
			return data;
		}
		else {
			std::string data(buf, 1);
			return data;
		}

	}

	int request_pty() 
	{
		int rc = libssh2_channel_request_pty(channel, "xterm");
		if(rc == LIBSSH2_ERROR_EAGAIN) {
			;
		}
		else if (rc) {
			int err = libssh2_session_last_errno(session);
			fprintf(stderr, "request pty failed: %d %d\n", rc, err);
		}
		else {
			fprintf(stderr, "request pty ok\n");
		}
		return 0;
	}

	int setenv(std::string name, std::string value)
	{
		return libssh2_channel_setenv(channel, 
					name.c_str(), value.c_str());
	}

	int shell() 
	{
		int rc = libssh2_channel_shell(channel);
		if(rc == LIBSSH2_ERROR_EAGAIN) {
			;
		}
		else if (rc) {
			int err = libssh2_session_last_errno(session);
			printf("get shell failed: %d %d\n", rc, err);
		}
		else {
			fprintf(stderr, "shell done\n");
		}
		return rc;
	}

	int write(std::string cmd) {
		int len = libssh2_channel_write(channel, 
				cmd.c_str(), 
				cmd.length());
		return 0;
	}

	int write_stderr(std::string cmd) {
                int len = libssh2_channel_write_stderr(channel, 
				cmd.c_str(), 
				cmd.length());
                return 0;
        }

private:
	LIBSSH2_SESSION *session;
	LIBSSH2_CHANNEL *channel;
};

#endif /* ~_SSH2_CHANNEL_H_ */

