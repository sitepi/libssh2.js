/*
 *
 */

#include "ssh2-session.h"

CHANNEL SESSION::channel()
{
	LIBSSH2_CHANNEL *channel = libssh2_channel_open_session(session);
	if(!channel) {
		int err = libssh2_session_last_errno(session);
		if(err == LIBSSH2_ERROR_EAGAIN) {
			;
		}
		else {
			fprintf(stderr, "Unable to open a session\n");
		}
	}
	else {
		fprintf(stderr, "channel ok\n");
	}

	return CHANNEL(session, channel);
}

SFTP SESSION::sftp()
{
	LIBSSH2_SFTP *sf = libssh2_sftp_init(session);
	if(!sf) {
		int err = libssh2_session_last_errno(session);
		if(err == LIBSSH2_ERROR_EAGAIN) {
			fprintf(stderr, "sftp failed eagain\n");;
		}
		else {
			fprintf(stderr, "Unable to init SFTP: (%d)\n", err);
		}
	}
	else {
		fprintf(stderr, "sftp init done\n");
	}

	return SFTP(session, sf);
}

int SESSION::userauth() 
{
	/* At this point we havn't authenticated. The first thing to do is check
	 * the hostkey's fingerprint against our known hosts Your app may have it
	 * hard coded, may go to a file, may present it to the user, that's your
	 * call
	 */
	char *userauthlist;

	/* check what authentication methods are available */
	userauthlist = libssh2_userauth_list(session, username.c_str(), username.length());
	fprintf(stderr, "Authentication methods: %s\n", userauthlist);
	return 0;
}

int SESSION::login()
{
	/* We could authenticate via password */
	if(libssh2_userauth_password(session, 
				username.c_str(), 
				password.c_str())) {
		int err = libssh2_session_last_errno(session);
		if(err == LIBSSH2_ERROR_EAGAIN) {
			;
		}
		else {
			fprintf(stderr, "\tAuthentication by password failed!\n");
		}
	}
	else {
		fprintf(stderr, "\tAuthentication by password succeeded.\n");
	}
	return 0;
}

