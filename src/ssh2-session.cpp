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

#include "ssh2-session.h"

CHANNEL SESSION::channel()
{
	LIBSSH2_CHANNEL *ch = NULL;

	if(!has_logined) {
		;
	}
	else if(has_opened) {
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
	}

	return CHANNEL(session, ch);
}

int SESSION::handshake()
{
	int rc = libssh2_session_handshake(session, fd);

	if(!rc) {
		const char *rsakey;
		rsakey = libssh2_hostkey_hash(session, 
					LIBSSH2_HOSTKEY_HASH_SHA1);
		int n = 0;
		char buff[256];
		for(int i = 0; i < 20; i++) {
			n += sprintf(buff + n, "%02X:", (unsigned char)rsakey[i]);
		}

		fingerprint = std::string(buff, n-1);
		has_opened = true;
	}
	else if(rc != LIBSSH2_ERROR_EAGAIN) {
		fprintf(stderr, "Failure establishing SSH session\n");
		error = rc;
	}

	return rc;
}

SFTP SESSION::sftp()
{
	LIBSSH2_SFTP *sf = NULL;

	if(!has_logined) {
		;
	}
	else if(has_opened) {
		sf = libssh2_sftp_init(session);
		if(!sf) {
			int e = libssh2_session_last_errno(session);
			if(e != LIBSSH2_ERROR_EAGAIN) {
				error = e;
				//fprintf(stderr, "Unable to init SFTP: (%d)\n", e);
			}
		}
	}

	return SFTP(session, sf);
}

std::string SESSION::userauth(std::string user) 
{
	char *m;
	std::string methods;
	int rc = -1;
	
	if(has_opened) {
		/* check what authentication methods are available */
		m = libssh2_userauth_list(session, user.c_str(), user.length());
		if(!m) {
			rc = libssh2_session_last_errno(session);
			error = rc;
		}
		else {
			fprintf(stderr, "Authentication methods: %s\n", m);
			methods = m;
			rc = 0;
		}
	}
	return methods;
}

int SESSION::login(std::string user, std::string passwd)
{
	int rc = -1;
	if(has_opened) {
		/* We could authenticate via password */
		rc = libssh2_userauth_password(session, user.c_str(), passwd.c_str());
		if(!rc) {
			has_logined = true;
		}
	}
	return rc;
}
