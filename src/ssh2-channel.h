/*
 *
 */
#ifndef _SSH2_CHANNEL_H_
#define _SSH2_CHANNEL_H_

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <emscripten/bind.h>

#define BUFF_LEN 4096

class CHANNEL {
public:
	CHANNEL(emscripten::val handle)
	{
		fprintf(stderr, "CHANNEL CANNOT CREATE DIRECTLY!\n");
	}

	CHANNEL()
	{
		fprintf(stderr, "CHANNEL CANNOT CREATE DIRECTLY!\n");
	}

	CHANNEL(LIBSSH2_SESSION *sess, LIBSSH2_CHANNEL *ch) :
			session(sess),
			channel(ch)
	{
		if(channel) {
			fprintf(stderr, "CHANNEL ok\n");
			good = true;
		}
	}

	int close() 
	{
		if(!channel) {
			fprintf(stderr, "channel error\n");
			return 0;
		}

		int rc = libssh2_channel_close(channel);
		if(!rc) {
			libssh2_channel_free(channel);
			channel = NULL;
		}
		return rc;
	}

	int eof() 
	{
		if(!channel) {
			fprintf(stderr, "channel error\n");
			return 0;
		}

		return libssh2_channel_eof(channel);
	}

	int exec(std::string cmd) 
	{
		if(!channel) {
			fprintf(stderr, "channel error\n");
			return 0;
		}
		return libssh2_channel_exec(channel, cmd.c_str());
	}

	int flush() 
	{
		if(!channel) {
			fprintf(stderr, "channel error\n");
			return 0;
		}

		return libssh2_channel_flush(channel);
	}

	std::string read()
	{
		if(!channel) {
			fprintf(stderr, "channel error\n");
			return 0;
		}

		int n = libssh2_channel_read(channel, buffer, BUFF_LEN);
		return (n) ? std::string(buffer, n) : nodata;
	}

	std::string read_err()
	{
		if(!channel) {
			fprintf(stderr, "channel error\n");
			return 0;
		}

		int n=libssh2_channel_read_stderr(channel, buffer, BUFF_LEN);
		return (n) ? std::string(buffer, n) : nodata;
	}

	int pty() 
	{
		if(!channel) {
			fprintf(stderr, "channel error\n");
			return 0;
		}

		int rc = libssh2_channel_request_pty(channel, "xterm");
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
		return rc;
	}

	int setenv(std::string name, std::string value)
	{
		if(!channel) {
			fprintf(stderr, "channel error\n");
			return 0;
		}

		return libssh2_channel_setenv(channel, 
					name.c_str(), value.c_str());
	}

	int shell() 
	{
		if(!channel) {
			fprintf(stderr, "channel error\n");
			return 0;
		}

		int rc = libssh2_channel_shell(channel);
		if(rc == LIBSSH2_ERROR_EAGAIN) {
				;
		}
		else if (rc) {
			int err = libssh2_session_last_errno(session);
			fprintf(stderr, "shell failed: %d %d\n", rc, err);
		}
		else {
			fprintf(stderr, "shell done\n");
		}
		return rc;
	}

	int write(std::string cmd)
	{
		if(!channel) {
			fprintf(stderr, "channel error\n");
			return 0;
		}

		return libssh2_channel_write(channel, cmd.c_str(),cmd.length());
	}

	int write_stderr(std::string cmd) {
		if(!channel) {
			fprintf(stderr, "channel error\n");
			return 0;
		}

		return libssh2_channel_write_stderr(channel,
					cmd.c_str(), 
					cmd.length());
	}

private:
	LIBSSH2_SESSION *session;
	LIBSSH2_CHANNEL *channel;

	char buffer[BUFF_LEN];
	std::string nodata;

	bool good = false;
};

#endif /* ~_SSH2_CHANNEL_H_ */

