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
#include <cstring>
#include <string>

#include "ssh2-common.h"
#include "ssh2-sftp-handle.h"

class SFTP {
public:
	SFTP(emscripten::val v)
		: session(nullptr)
		, sftp(nullptr)
		, active(false)
		, error(0)
	{
		std::memset(buffer, 0, sizeof(buffer));
		std::memset(&attrs, 0, sizeof(attrs));
		std::memset(&st, 0, sizeof(st));
	}

	SFTP(LIBSSH2_SESSION* sess, LIBSSH2_SFTP* sf)
		: session(sess)
		, sftp(sf)
		, active(sf != nullptr)
		, error(0)
	{
		std::memset(buffer, 0, sizeof(buffer));
		std::memset(&attrs, 0, sizeof(attrs));
		std::memset(&st, 0, sizeof(st));
	}

	// Helper function to check if active
	inline bool checkActive() const noexcept {
		return active && sftp != nullptr;
	}

	LIBSSH2_SFTP_ATTRIBUTES lstat(const std::string& path)
	{
		std::memset(&attrs, 0, sizeof(attrs));
		if (!checkActive()) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
			return attrs;
		}

		const int rc = libssh2_sftp_lstat(sftp, path.c_str(), &attrs);
		error = rc;
		return attrs;
	}

	int mkdir(const std::string& path, long mode) 
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_SOCKET_NONE;
		}
		const auto len = static_cast<unsigned int>(path.length());
		return libssh2_sftp_mkdir_ex(sftp, path.c_str(), len, mode);
	}

	SFTP_HANDLE open(const std::string& path, 
	                 unsigned long flags, long mode, int type) 
	{
		LIBSSH2_SFTP_HANDLE* h = nullptr;
		if (checkActive()) {
			h = libssh2_sftp_open_ex(sftp, 
			                        path.c_str(), 
			                        static_cast<unsigned int>(path.length()),
			                        flags, mode, type);
		}
		if (!h) {
			error = session ? libssh2_session_last_errno(session) : LIBSSH2_ERROR_SOCKET_NONE;
			if (error == LIBSSH2_ERROR_SFTP_PROTOCOL && sftp) {
				error = libssh2_sftp_last_error(sftp);
			}
		} else {
			error = 0;
		}
		return SFTP_HANDLE(sftp, h);
	}

	SFTP_HANDLE opendir(const std::string& path) 
	{
		LIBSSH2_SFTP_HANDLE* h = nullptr;
		if (checkActive()) {
			h = libssh2_sftp_opendir(sftp, path.c_str());
		}
		if (!h) {
			error = session ? libssh2_session_last_errno(session) : LIBSSH2_ERROR_SOCKET_NONE;
			if (error == LIBSSH2_ERROR_SFTP_PROTOCOL && sftp) {
				error = libssh2_sftp_last_error(sftp);
			}
		} else {
			error = 0;
		}
		return SFTP_HANDLE(sftp, h);
	}

	std::string readlink(const std::string& path) 
	{
		if (!checkActive()) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
			return std::string();
		}

		const int n = libssh2_sftp_readlink(sftp, path.c_str(), buffer, BUFF_LEN);
		error = (n < 0) ? n : 0;
		return (n > 0) ? std::string(buffer, static_cast<size_t>(n)) : std::string();
	}

	int unlink(const std::string& path)
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_SOCKET_NONE;
		}
		const auto len = static_cast<unsigned int>(path.length());
		return libssh2_sftp_unlink_ex(sftp, path.c_str(), len);
	}

	std::string realpath(const std::string& path)
	{
		if (!checkActive()) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
			return std::string();
		}

		const int n = libssh2_sftp_realpath(sftp, path.c_str(), buffer, BUFF_LEN);
		error = (n < 0) ? n : 0;
		return (n > 0) ? std::string(buffer, static_cast<size_t>(n)) : std::string();
	}

	int rename(const std::string& source, const std::string& dest, long flags)
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_SOCKET_NONE;
		}
		const auto src_len = static_cast<unsigned int>(source.length());
		const auto dst_len = static_cast<unsigned int>(dest.length());
		return libssh2_sftp_rename_ex(sftp, source.c_str(), src_len,
		                               dest.c_str(), dst_len, flags);
	}

	int rmdir(const std::string& path)
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_SOCKET_NONE;
		}
		const auto len = static_cast<unsigned int>(path.length());
		return libssh2_sftp_rmdir_ex(sftp, path.c_str(), len);
	}

	int setstat(const std::string& path)
	{
		return checkActive() ? libssh2_sftp_setstat(sftp, path.c_str(), nullptr)
		                     : LIBSSH2_ERROR_SOCKET_NONE;
	}

	int shutdown()
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_SOCKET_NONE;
		}
		const int rc = libssh2_sftp_shutdown(sftp);
		if (rc == 0) {
			sftp = nullptr;
			active = false;
		}
		return rc;
	}

	LIBSSH2_SFTP_ATTRIBUTES stat(const std::string& path, 
	                              int type = LIBSSH2_SFTP_STAT)
	{
		std::memset(&attrs, 0, sizeof(attrs));
		if (!checkActive()) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
			return attrs;
		}

		const int rc = libssh2_sftp_stat_ex(sftp,
		                                    path.c_str(), 
		                                    static_cast<unsigned int>(path.length()),
		                                    type, &attrs);
		error = rc;
		return attrs;
	}

	LIBSSH2_SFTP_STATVFS statvfs(const std::string& path)
	{
		std::memset(&st, 0, sizeof(st));
		if (!checkActive()) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
			return st;
		}

		const int rc = libssh2_sftp_statvfs(sftp,
		                                    path.c_str(),
		                                    static_cast<size_t>(path.length()),
		                                    &st);
		error = rc;
		return st;
	}

	std::string symlink(const std::string& orig, const std::string& dest, 
	                    int type = LIBSSH2_SFTP_SYMLINK)
	{
		if (!checkActive()) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
			return "";
		}

		const int n = libssh2_sftp_symlink_ex(sftp, 
		                                      orig.c_str(),
		                                      static_cast<unsigned int>(orig.length()),
		                                      buffer,
		                                      BUFF_LEN,
		                                      type);
		error = (n < 0) ? n : 0;
		return (n > 0) ? std::string(buffer, static_cast<size_t>(n)) : "";
	}

	bool getActive() const noexcept {
		return active;
	}

	int getError() const noexcept {
		return error;
	}

private:
	LIBSSH2_SFTP* sftp = nullptr;
	LIBSSH2_SESSION* session = nullptr;

	LIBSSH2_SFTP_ATTRIBUTES attrs;
	LIBSSH2_SFTP_STATVFS st;

	char buffer[BUFF_LEN] = {0};
	bool active = false;
	int error = 0;
};

#endif /* ~_SSH2_SFTP_H_ */
