/*
 *
 */

#include "ssh2-session.h"

extern "C" {
extern int waitsocket(int fd);
}

CHANNEL SESSION::channel()
{
	LIBSSH2_CHANNEL *ch = NULL;

	if(!has_opened) {
		fprintf(stderr, "session error\n");
		return CHANNEL(session, ch);
	}

	ch = libssh2_channel_open_session(session);
	if(!ch) {
		int err = libssh2_session_last_errno(session);
		if(err == LIBSSH2_ERROR_EAGAIN) {
			;
		}
		else {
			fprintf(stderr, "Unable to open a session\n");
		}
	}

	return CHANNEL(session, ch);
}

int SESSION::handshake()
{
	int rc = libssh2_session_handshake(session, fd);

	if(rc == LIBSSH2_ERROR_EAGAIN) {
		waitsocket(fd);
	}
	else if(rc) {
		int err = libssh2_session_last_errno(session);
		fprintf(stderr, "handshake failed: (%d) %d\n", err, rc);
		has_error = true;
	}
	else {
		fprintf(stderr, "handshake ok\n");
		const char *rsakey;
		rsakey = libssh2_hostkey_hash(session, 
					LIBSSH2_HOSTKEY_HASH_SHA1);
		int n = 0;
		char buff[256];
		for(int i = 0; i < 20; i++) {
			n += sprintf(buff + n, "%02X:", (char)rsakey[i]);
		}

		fingerprint = std::string(buff, n-1);

		fprintf(stderr, "Fingerprint: %s\n", fingerprint.c_str());

		has_opened = true;
	}

	return rc;
}

SFTP SESSION::sftp()
{
	LIBSSH2_SFTP *sf = NULL;
	if(!has_opened) {
		fprintf(stderr, "session error\n");
		return SFTP(session, sf);
	}

	sf = libssh2_sftp_init(session);
	if(!sf) {
		int err = libssh2_session_last_errno(session);
		if(err != LIBSSH2_ERROR_EAGAIN) {
			fprintf(stderr, "Unable to init SFTP: (%d)\n", err);
		}
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
	char *m;
	int rc = -1;
	
	if(!has_opened) {
		fprintf(stderr, "session error\n");
		return rc;
	}

	/* check what authentication methods are available */
	m = libssh2_userauth_list(session, user.c_str(), user.length());
	if(!m) {
		rc = libssh2_session_last_errno(session);
	}
	else {
		fprintf(stderr, "Authentication methods: %s\n", m);
		methods = m;
		rc = 0;
	}
	return rc;
}

int SESSION::login()
{
	int rc = -1;
	if(!has_opened) {
                fprintf(stderr, "session error\n");
                return rc;
        }

	/* We could authenticate via password */
	rc = libssh2_userauth_password(session, user.c_str(), passwd.c_str());
	if(rc) {
		fprintf(stderr, "Authentication by password succeeded.\n");
	}
	return rc;
}
