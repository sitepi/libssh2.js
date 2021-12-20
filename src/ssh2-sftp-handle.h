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

#ifndef _SSH2_SFTP_HANDLE_H_
#define _SSH2_SFTP_HANDLE_H_

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <emscripten/bind.h>

#ifndef BUFF_LEN
#define BUFF_LEN 4096
#endif

class SFTP_HANDLE {
public:
	SFTP_HANDLE(emscripten::val handle)
	{
		fprintf(stderr, "SFTP_HANDLE CANNOT CREATE DIRECTLY!\n");
	}

	SFTP_HANDLE(LIBSSH2_SFTP *sf, LIBSSH2_SFTP_HANDLE *hand) :
		sftp(sf),
		handle(hand)
	{
		if(handle) {
			fprintf(stderr, "SFTP_HANDLE ok\n");
			active = true;
			memset(&attrs, '\0', sizeof(attrs));
			memset(&st, '\0', sizeof(st));
		}
	}

	int close()
	{
		if(active) {
			return libssh2_sftp_close(handle);
		}
		return LIBSSH2_ERROR_SOCKET_NONE;
	}

	int closedir()
	{
		if(active) {
			return libssh2_sftp_closedir(handle);
		}
		return  LIBSSH2_ERROR_SOCKET_NONE;
	}

	LIBSSH2_SFTP_ATTRIBUTES fsetstat()
	{
		memset(&attrs, '\0', sizeof(attrs));
		error = LIBSSH2_ERROR_SOCKET_NONE;
		if(active) {
			error = libssh2_sftp_fsetstat(handle, &attrs);
		}
		return attrs;
	}

	LIBSSH2_SFTP_ATTRIBUTES fstat(int setstat = 0)
	{
		memset(&attrs, '\0', sizeof(attrs));
		error = LIBSSH2_ERROR_SOCKET_NONE;

		if(active) {
			error = libssh2_sftp_fstat_ex(handle, &attrs, setstat);
		}
		return attrs;
	}

	LIBSSH2_SFTP_STATVFS fstatvfs() 
	{
		memset(&attrs, '\0', sizeof(attrs));
		error = LIBSSH2_ERROR_SOCKET_NONE;
		if(active) {
			error = libssh2_sftp_fstatvfs(handle,  &st);
		}
		return st;
	}

	int fsync()
	{
		if(active) {
			return libssh2_sftp_fsync(handle);
		}
		return LIBSSH2_ERROR_SOCKET_NONE;
	}

	std::string read() 
	{
		int n = 0;
		error = LIBSSH2_ERROR_SOCKET_NONE;
		if(active) {
			n = libssh2_sftp_read(handle, buffer, BUFF_LEN);
			error = (n <0) ? n : 0;
		}
		return (n>0) ? std::string(buffer, n) : nodata;
	}

	std::string readdir() 
	{
		int n = 0;
		memset(&attrs, '\0', sizeof(attrs));
		error = LIBSSH2_ERROR_SOCKET_NONE;
		if(active) {
			n = libssh2_sftp_readdir(handle, buffer, BUFF_LEN, &attrs);
			error = (n<0) ? n : 0;
		}
		
		return (n>0) ? std::string(buffer, n) : nodata;
	}

	std::string readdir_ex() 
	{
		int n = 0;
		error = LIBSSH2_ERROR_SOCKET_NONE;
		if(active) {
			n = libssh2_sftp_readdir_ex(handle, NULL, 0, 
					NULL, 0, NULL);
			error = (n<0) ? n : 0;
		}

		return (n>0) ? std::string(buffer, n) : nodata;
	}

	int rewind() 
	{
		if(active) {
			libssh2_sftp_rewind(handle);
			return LIBSSH2_ERROR_NONE;
		}
		return LIBSSH2_ERROR_SOCKET_NONE;
	}

	int seek(size_t offset) 
	{
		if(active) {
			libssh2_sftp_seek(handle, offset);
			return LIBSSH2_ERROR_NONE;
		}
		return LIBSSH2_ERROR_SOCKET_NONE;
	}

	int seek64(libssh2_uint64_t offset)
	{
		if(active) {
			libssh2_sftp_seek64(handle, offset);
			return LIBSSH2_ERROR_NONE;
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

	int tell()
	{
		if(active) {
			libssh2_sftp_tell(handle);
			return LIBSSH2_ERROR_NONE;
		}
		return LIBSSH2_ERROR_SOCKET_NONE;
	}

	int tell64()
	{
		if(active) {
			libssh2_sftp_tell64(handle);
			return LIBSSH2_ERROR_NONE;
		}
		return LIBSSH2_ERROR_SOCKET_NONE;
	}

	int write(std::string buff)
	{
		if(active) {
			return libssh2_sftp_write(handle, 
					buff.c_str(), buff.length());
		}
		return LIBSSH2_ERROR_SOCKET_NONE;
	}

	bool getActive() const {
		return active;
	}

	int getError() const {
		return error;
	}

private:
	emscripten::val & attrs_object(emscripten::val &v, 
			const LIBSSH2_SFTP_ATTRIBUTES *attrs)
	{
		v.set("flags", attrs->flags);
		v.set("filesize", attrs->filesize);
		v.set("uid", attrs->uid);
		v.set("gid", attrs->gid);
		v.set("perm", attrs->permissions);
		v.set("atime", attrs->atime);
		v.set("mtime", attrs->mtime);

		return v;
	}
private:
	LIBSSH2_SFTP_HANDLE *handle;
	LIBSSH2_SESSION     *session;
	LIBSSH2_SFTP        *sftp;

	LIBSSH2_SFTP_ATTRIBUTES attrs;
	LIBSSH2_SFTP_STATVFS st;

	char buffer[BUFF_LEN];
	std::string nodata;

	bool active = false;
	int error = 0;
};

#endif /* ~_SSH2_SFTP_HANDLE_H_ */

