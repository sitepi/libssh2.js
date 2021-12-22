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
#include "ssh2-channel.h"
#include "ssh2-sftp.h"
#include "ssh2-sftp-handle.h"

static bool _init = false;

static int ssh2_init(int flags)
{
	if(_init)
		return 0;

	int rc = libssh2_init(flags);
	_init = (rc) ? false: true;
	#if 0
	if(_init) {
		printf("libssh2-%s loaded\n", libssh2_version(0));
	}
	#endif
	return rc;
}

static void ssh2_exit()
{
	libssh2_exit();
	_init = false;
	//printf("libssh2-%s exit\n", libssh2_version(0));
}

static std::string ssh2_version()
{
	return std::string(libssh2_version(0));
}

// Binding code
EMSCRIPTEN_BINDINGS(libssh2_js) {
	emscripten::function("init", &ssh2_init);
	emscripten::function("exit", &ssh2_exit);
	emscripten::function("version", &ssh2_version);

	emscripten::class_<CHANNEL>("_CHANNEL")
		.constructor<emscripten::val>()

		.property("active", &CHANNEL::getActive)

		.function("close", &CHANNEL::close)
		.function("eof", &CHANNEL::eof)
		.function("exec", &CHANNEL::exec)
		.function("flush", &CHANNEL::flush)
		.function("read", &CHANNEL::read)
		.function("read_err", &CHANNEL::read_err)
		.function("pty", &CHANNEL::pty)
		.function("pty_size", &CHANNEL::pty_size)
		.function("setenv", &CHANNEL::setenv)
		.function("shell", &CHANNEL::shell)
		.function("write", &CHANNEL::write)
		.function("write_err", &CHANNEL::write_err)
		.function("x11_req", &CHANNEL::x11_req)
		;

	emscripten::value_object<LIBSSH2_SFTP_ATTRIBUTES>("attrs")
		.field("flags", &LIBSSH2_SFTP_ATTRIBUTES::flags)
		.field("filesize", &LIBSSH2_SFTP_ATTRIBUTES::filesize)
		.field("uid", &LIBSSH2_SFTP_ATTRIBUTES::uid)
		.field("gid", &LIBSSH2_SFTP_ATTRIBUTES::gid)
		.field("perm", &LIBSSH2_SFTP_ATTRIBUTES::permissions)
		.field("atime", &LIBSSH2_SFTP_ATTRIBUTES::atime)
		.field("mtime", &LIBSSH2_SFTP_ATTRIBUTES::mtime)

		;
	emscripten::value_object<LIBSSH2_SFTP_STATVFS>("statvfs")
		.field("bsize", &LIBSSH2_SFTP_STATVFS::f_bsize)
		.field("frsize", &LIBSSH2_SFTP_STATVFS::f_frsize)
		.field("blocks", &LIBSSH2_SFTP_STATVFS::f_blocks)
		.field("bfree", &LIBSSH2_SFTP_STATVFS::f_bfree)
		.field("bavail", &LIBSSH2_SFTP_STATVFS::f_bavail)
		.field("files", &LIBSSH2_SFTP_STATVFS::f_files)
		.field("ffree", &LIBSSH2_SFTP_STATVFS::f_ffree)
		.field("favail", &LIBSSH2_SFTP_STATVFS::f_favail)
		.field("fsid", &LIBSSH2_SFTP_STATVFS::f_fsid)
		.field("flag", &LIBSSH2_SFTP_STATVFS::f_flag)
		.field("namemax", &LIBSSH2_SFTP_STATVFS::f_namemax)
		;

	emscripten::class_<SFTP_HANDLE>("_SFTP_HANDLE")
		.constructor<emscripten::val>()

		.property("active", &SFTP_HANDLE::getActive)
		.property("error", &SFTP_HANDLE::getError)

		.function("close", &SFTP_HANDLE::close)
		.function("closedir", &SFTP_HANDLE::closedir)
		.function("fsetstat", &SFTP_HANDLE::fsetstat)
		.function("fstat", &SFTP_HANDLE::fstat)
		.function("fstatvfs", &SFTP_HANDLE::fstatvfs)
		.function("fsync", &SFTP_HANDLE::fsync)
		.function("read", &SFTP_HANDLE::read)
		.function("readdir", &SFTP_HANDLE::readdir)
		.function("readdir_ex", &SFTP_HANDLE::readdir_ex)
		.function("rewind", &SFTP_HANDLE::rewind)
		.function("seek", &SFTP_HANDLE::seek)
		.function("seek64", &SFTP_HANDLE::seek64)
		.function("tell", &SFTP_HANDLE::tell)
		.function("tell64", &SFTP_HANDLE::tell64)
		.function("write", &SFTP_HANDLE::write)
		;

	emscripten::class_<SFTP>("_SFTP")
		.constructor<emscripten::val>()

		.property("active", &SFTP::getActive)
		.property("error", &SFTP::getError)

		.function("lstat", &SFTP::lstat)
		.function("mkdir", &SFTP::mkdir)
		.function("open", &SFTP::open)
		.function("opendir", &SFTP::opendir)
		.function("readlink", &SFTP::readlink)
		.function("realpath", &SFTP::realpath)
		.function("rename", &SFTP::rename)
		.function("rmdir", &SFTP::rmdir)
		.function("setstat", &SFTP::setstat)
		.function("shutdown", &SFTP::shutdown)
		.function("stat", &SFTP::stat)
		.function("statvfs", &SFTP::statvfs)
		.function("symlink", &SFTP::symlink)
		.function("unlink", &SFTP::unlink)
		;

	emscripten::class_<SESSION>("_SESSION")
		.constructor<emscripten::val>()

		.property("fingerprint", &SESSION::getFingerprint)
		.property("send", &SESSION::getSendCb, &SESSION::setSendCb)
		.property("error", &SESSION::getError)

		.function("pushdata", &SESSION::pushdata)
		.function("userauth", &SESSION::userauth)
		.function("login", &SESSION::login)
		.function("channel", &SESSION::channel)
		.function("sftp", &SESSION::sftp)
		.function("tcpip", &SESSION::tcpip)
		;
}
