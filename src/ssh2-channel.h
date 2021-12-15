/*
 * Sample showing how to do SSH2 connect.
 *
 * The sample code has default values for host name, user name, password
 * and path to copy, but you can specify them on the command line like:
 *
 * "ssh2 host user password [-p|-i|-k]"
 */
#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>

#include <emscripten.h>
#include <emscripten/bind.h>

static bool libssh2_loaded = false;

class CHANNEL {
public:
	CHANNEL(emscripten::val handler)
	{
		channel = libssh2_channel_open_session(session);
		if(!channel) {
			fprintf(stderr, "Unable to open a session\n");
		}
		else {
			fprintf(stderr, "channel ok\n");
		}
	}

	int get_exit_signal() {
		return libssh2_channel_get_exit_signal(channel, NULL, NULL, NULL, NULL, NULL, NULL);
	}

	int get_exit_status() {
		return libssh2_channel_get_exit_status(channel);
	}

    	int close() {
		return libssh2_channel_close(channel);
	}

	int eof() {
		return libssh2_channel_eof(channel);
	}

	int exec() {
		return libssh2_channel_exec(channel, NULL);
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

	int read_stderr() {
		return libssh2_channel_read_stderr(channel, NULL, 0);
	}

	int request_pty() 
	{
		int rc = libssh2_channel_request_pty(channel, "vanilla");
		if(rc == LIBSSH2_ERROR_EAGAIN) {

		}
		else if (rc) {
			int err = libssh2_session_last_errno(session);
			printf("request pty failed: %d %d\n", rc, err);
		}
		else {
			fprintf(stderr, "request pty ok\n");
		}
		return 0;
	}

	int setenv() {
		return libssh2_channel_setenv(channel, NULL, NULL);
	}

	int shell() 
	{
		int rc = libssh2_channel_shell(channel);
		if(rc == LIBSSH2_ERROR_EAGAIN) {

		}
		else if (rc) {
			int err = libssh2_session_last_errno(session);
			printf("request pty failed: %d %d\n", rc, err);
		}
		else {
			fprintf(stderr, "open shell done\n");
		}
		return rc;
	}

	int write(std::string cmd) {
		int len = libssh2_channel_write(channel, cmd.c_str(), cmd.length());
		return 0;
	}

	int write_stderr(std::string cmd) {
                int len = libssh2_channel_write_stderr(channel, cmd.c_str(), cmd.length());
                return 0;
        }

private:
	LIBSSH2_SESSION *session;
	LIBSSH2_CHANNEL *channel;
};
