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

#ifndef _SSH2_SFTP_H_
#define _SSH2_SFTP_H_

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <emscripten/bind.h>

#include "ssh2-sftp-handle.h"

#ifndef BUFF_LEN
#define BUFF_LEN 4096
#endif

class SFTP {
public:
	SFTP(emscripten::val v)
	{

	}

	SFTP(LIBSSH2_SESSION *sess, LIBSSH2_SFTP *sf) :
			session(sess),
			sftp(sf)
	{
		if(sftp) {
			//fprintf(stderr, "SFTP ok\n");
			active = true;
		}
	}

	LIBSSH2_SFTP_ATTRIBUTES lstat(std::string path)
	{
		int rc = LIBSSH2_ERROR_SOCKET_NONE;
		memset(&attrs, '\0', sizeof(attrs));

		if(active) {
			rc = libssh2_sftp_lstat(sftp, path.c_str(), &attrs);
			error = (rc) ? rc : 0;
		}
		
		return attrs;
	}

	int mkdir(std::string path, long mode) 
	{
		if(active) {
			return libssh2_sftp_mkdir_ex(sftp,
				path.c_str(), path.length(), mode);
		}
		return LIBSSH2_ERROR_SOCKET_NONE;
	}

	SFTP_HANDLE open(std::string path, 
				unsigned long flags, long mode, int type) 
	{
		LIBSSH2_SFTP_HANDLE *h = NULL;
		if(active) {
			h = libssh2_sftp_open_ex(sftp, 
						path.c_str(), 
						path.length(),
						flags, mode, type);
		}
		error = (!h) ? libssh2_session_last_errno(session) : 0;
		error = (error == LIBSSH2_ERROR_SFTP_PROTOCOL) ? libssh2_sftp_last_error(sftp) : error;
		return SFTP_HANDLE(sftp, h);
	}

	SFTP_HANDLE opendir(std::string path) 
	{
		LIBSSH2_SFTP_HANDLE *h = NULL;
		if(active) {
			h = libssh2_sftp_opendir(sftp, path.c_str());
		}
		error = (!h) ? libssh2_session_last_errno(session) : 0;
		error = (error == LIBSSH2_ERROR_SFTP_PROTOCOL) ? libssh2_sftp_last_error(sftp) : error;
		return SFTP_HANDLE(sftp, h);
	}

	std::string readlink(std::string path) 
	{
		int n = 0;
		if(active) {
			n = libssh2_sftp_readlink(sftp, 
					path.c_str(), buffer, BUFF_LEN);
		}
		error = (n<0) ? n : 0;
		return (n>0) ? std::string(buffer, n) : nodata;
	}

	int unlink(std::string path)
	{
		if(active) {
			return libssh2_sftp_unlink_ex(sftp,
					path.c_str(), path.length());
		}
		return LIBSSH2_ERROR_SOCKET_NONE;
	}

	std::string realpath(std::string path)
	{
		int n = 0;
		if(active) {
			n = libssh2_sftp_realpath(sftp, 
					path.c_str(), buffer, BUFF_LEN);
		}
		error = (n<0) ? n : 0;
		return (n>0) ? std::string(buffer, n) : nodata;
	}

	int rename(std::string source, std::string dest, long flags)
	{
		if(active) {
			return libssh2_sftp_rename_ex(sftp,
						source.c_str(),
						source.length(),
						dest.c_str(),
						dest.length(),
						flags);
		}
		return LIBSSH2_ERROR_SOCKET_NONE;
	}

	int rmdir(std::string path)
	{
		if(active) {
			return libssh2_sftp_rmdir_ex(sftp, path.c_str(), path.length());
		}
		return LIBSSH2_ERROR_SOCKET_NONE;
	}

	int setstat(std::string path)
	{
		if(active) {
			return libssh2_sftp_setstat(sftp, path.c_str(), NULL);
		}
		return LIBSSH2_ERROR_SOCKET_NONE;
	}

	int shutdown()
	{
		if(active) {
			return libssh2_sftp_shutdown(sftp);
		}
		return LIBSSH2_ERROR_SOCKET_NONE;
	}

	LIBSSH2_SFTP_ATTRIBUTES stat(std::string path, 
				int type = LIBSSH2_SFTP_STAT)
	{
		int rc = 0;

		memset(&attrs, '\0', sizeof(attrs));
		error = LIBSSH2_ERROR_SOCKET_NONE;
		if(active) {
			rc = libssh2_sftp_stat_ex(sftp,
						path.c_str(), 
						path.length(),
						type, &attrs);
			error = (rc) ? rc : 0;
		}
		return attrs;
	}

	LIBSSH2_SFTP_STATVFS statvfs(std::string path)
	{
		int rc = 0;
		memset(&attrs, '\0', sizeof(attrs));
		error = LIBSSH2_ERROR_SOCKET_NONE;
		if(active) {
			rc = libssh2_sftp_statvfs(sftp,
						path.c_str(),
						path.length(),
						&st);
			error = (rc) ? rc : 0;
		}

		return st;
	}

	std::string symlink(std::string orig, std::string dest, 
				int type = LIBSSH2_SFTP_SYMLINK)
	{
		int n = 0;
		error = LIBSSH2_ERROR_SOCKET_NONE;
		if(active) {
			n= libssh2_sftp_symlink_ex(sftp, 
					orig.c_str(),
					orig.length(),
					buffer,
					BUFF_LEN,
					type);
			error = (n<0) ? n : 0;
		}
		return (n>0) ? std::string(buffer, n) : nodata;

	}

	bool getActive() const {
		return active;
	}

	int getError() const {
		return error;
	}

private:
	LIBSSH2_SFTP 	    *sftp;
	LIBSSH2_SESSION     *session;

	LIBSSH2_SFTP_ATTRIBUTES attrs;
	LIBSSH2_SFTP_STATVFS    st;

	char buffer[BUFF_LEN];
	std::string nodata;

	bool active = false;
	int error = 0;
};

#endif /* ~_SSH2_SFTP_H_ */
