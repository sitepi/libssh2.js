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
#include <cstring>
#include <string>

#include "ssh2-common.h"

class SFTP_HANDLE {
public:
	SFTP_HANDLE(emscripten::val handle)
		: handle(nullptr)
		, sftp(nullptr)
		, active(false)
		, error(0)
	{
		std::memset(buffer, 0, sizeof(buffer));
		std::memset(&attrs, 0, sizeof(attrs));
		std::memset(&st, 0, sizeof(st));
	}

	SFTP_HANDLE(LIBSSH2_SFTP* sf, LIBSSH2_SFTP_HANDLE* hand)
		: handle(hand)
		, sftp(sf)
		, active(hand != nullptr)
		, error(0)
	{
		std::memset(buffer, 0, sizeof(buffer));
		std::memset(&attrs, 0, sizeof(attrs));
		std::memset(&st, 0, sizeof(st));
	}

	// Helper function to check if active
	inline bool checkActive() const noexcept {
		return active && handle != nullptr;
	}

	int close()
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_SOCKET_NONE;
		}
		const int rc = libssh2_sftp_close(handle);
		if (rc == 0) {
			handle = nullptr;
			active = false;
		}
		return rc;
	}

	int closedir()
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_SOCKET_NONE;
		}
		const int rc = libssh2_sftp_closedir(handle);
		if (rc == 0) {
			handle = nullptr;
			active = false;
		}
		return rc;
	}

	LIBSSH2_SFTP_ATTRIBUTES fsetstat()
	{
		std::memset(&attrs, 0, sizeof(attrs));
		if (!checkActive()) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
			return attrs;
		}
		error = libssh2_sftp_fsetstat(handle, &attrs);
		return attrs;
	}

	LIBSSH2_SFTP_ATTRIBUTES fstat(int setstat = 0)
	{
		std::memset(&attrs, 0, sizeof(attrs));
		if (!checkActive()) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
			return attrs;
		}
		error = libssh2_sftp_fstat_ex(handle, &attrs, setstat);
		return attrs;
	}

	LIBSSH2_SFTP_STATVFS fstatvfs() 
	{
		std::memset(&st, 0, sizeof(st));
		if (!checkActive()) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
			return st;
		}
		error = libssh2_sftp_fstatvfs(handle, &st);
		return st;
	}

	int fsync()
	{
		return checkActive() ? libssh2_sftp_fsync(handle)
		                     : LIBSSH2_ERROR_SOCKET_NONE;
	}

	std::string read() 
	{
		if (!checkActive()) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
			return std::string();
		}

		const ssize_t n = libssh2_sftp_read(handle, buffer, BUFF_LEN);
		error = (n < 0) ? static_cast<int>(n) : 0;
		return (n > 0) ? std::string(buffer, static_cast<size_t>(n)) : std::string();
	}

	std::string readdir() 
	{
		if (!checkActive()) {
			std::memset(&attrs, 0, sizeof(attrs));
			error = LIBSSH2_ERROR_SOCKET_NONE;
			return std::string();
		}

		std::memset(&attrs, 0, sizeof(attrs));
		const int n = libssh2_sftp_readdir(handle, buffer, BUFF_LEN, &attrs);
		error = (n < 0) ? n : 0;
		return (n > 0) ? std::string(buffer, static_cast<size_t>(n)) : std::string();
	}

	std::string readdir_ex() 
	{
		if (!checkActive()) {
			error = LIBSSH2_ERROR_SOCKET_NONE;
			return std::string();
		}

		const int n = libssh2_sftp_readdir_ex(handle, nullptr, 0, nullptr, 0, nullptr);
		error = (n < 0) ? n : 0;
		return (n > 0) ? std::string(buffer, static_cast<size_t>(n)) : std::string();
	}

	int rewind() 
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_SOCKET_NONE;
		}
		libssh2_sftp_rewind(handle);
		return LIBSSH2_ERROR_NONE;
	}

	int seek(size_t offset) 
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_SOCKET_NONE;
		}
		libssh2_sftp_seek(handle, offset);
		return LIBSSH2_ERROR_NONE;
	}

	int seek64(libssh2_uint64_t offset)
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_SOCKET_NONE;
		}
		libssh2_sftp_seek64(handle, offset);
		return LIBSSH2_ERROR_NONE;
	}

	int shutdown()
	{
		return checkActive() ? libssh2_sftp_shutdown(sftp)
		                     : LIBSSH2_ERROR_SOCKET_NONE;
	}

	size_t tell() const
	{
		return checkActive() ? libssh2_sftp_tell(handle) : 0;
	}

	libssh2_uint64_t tell64() const
	{
		return checkActive() ? libssh2_sftp_tell64(handle) : 0;
	}

	int write(const std::string& buff)
	{
		if (!checkActive()) {
			return LIBSSH2_ERROR_SOCKET_NONE;
		}
		return libssh2_sftp_write(handle, buff.data(), buff.length());
	}

	bool getActive() const noexcept {
		return active;
	}

	int getError() const noexcept {
		return error;
	}

private:
	LIBSSH2_SFTP_HANDLE* handle = nullptr;
	LIBSSH2_SFTP* sftp = nullptr;

	LIBSSH2_SFTP_ATTRIBUTES attrs;
	LIBSSH2_SFTP_STATVFS st;

	char buffer[BUFF_LEN] = {0};
	bool active = false;
	int error = 0;
};

#endif /* ~_SSH2_SFTP_HANDLE_H_ */

