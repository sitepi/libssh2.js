/*
 *
 */

#include "ssh2-session.h"

CHANNEL SESSION::newchannel()
{
	channel = libssh2_channel_open_session(session);
	if(!channel) {
		fprintf(stderr, "Unable to open a session\n");
	}
	else {
		fprintf(stderr, "channel ok\n");
	}
	CHANNEL ch(session, channel);
	return ch;
}

SFTP SESSION::sftp()
{
	SFTP sftp(session);
	return sftp;
}

int SESSION::login1(emscripten::val cb) 
{
	/* At this point we havn't authenticated. The first thing to do is check
	 * the hostkey's fingerprint against our known hosts Your app may have it
	 * hard coded, may go to a file, may present it to the user, that's your
	 * call
	 */
	int auth_pw = 0;
	char *userauthlist;

	/* check what authentication methods are available */
	userauthlist = libssh2_userauth_list(session, username.c_str(), username.length());
	fprintf(stderr, "Authentication methods: %s\n", userauthlist);
	if(strstr(userauthlist, "password") != NULL) {
		auth_pw |= 1;
	}
	if(strstr(userauthlist, "keyboard-interactive") != NULL) {
		auth_pw |= 2;
	}
	if(strstr(userauthlist, "publickey") != NULL) {
		auth_pw |= 4;
	}
	return 0;
}

int SESSION::login(emscripten::val cb)
{
//	if(auth_pw & 1) {
		/* We could authenticate via password */
		if(libssh2_userauth_password(session, username.c_str(), password.c_str())) {
			fprintf(stderr, "\tAuthentication by password failed!\n");
		}
		else {
			fprintf(stderr, "\tAuthentication by password succeeded.\n");
		}
//	}
	return 0;
}

