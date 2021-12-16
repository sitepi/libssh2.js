/*
 *
 */

#include "ssh2-session.h"
#include "ssh2-channel.h"
#include "ssh2-sftp.h"
#include "ssh2-sftp-handle.h"

static bool ssh2_loaded = false;
static int ssh2_init(int flags)
{
	if(ssh2_loaded)
		return 0;

	int rc = libssh2_init(flags);
	if(rc != 0) {
		fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
	}
	else {
		fprintf(stderr, "libssh2-%s loaded\n", libssh2_version(0));
		ssh2_loaded = true;
	}
	return rc;
}

static void ssh2_exit()
{
	libssh2_exit();
	ssh2_loaded = false;
	fprintf(stderr, "libssh2-%s exit\n", libssh2_version(0));
}

// Binding code
EMSCRIPTEN_BINDINGS(libssh2_js) {
  emscripten::function("init", &ssh2_init);
  emscripten::function("exit", &ssh2_exit);

  emscripten::class_<CHANNEL>("CHANNEL")
    .constructor<emscripten::val>()
    
    .function("close", &CHANNEL::close)
    .function("eof", &CHANNEL::eof)
    .function("exec", &CHANNEL::exec)
    .function("flush", &CHANNEL::flush)
    .function("read", &CHANNEL::read)
    .function("read_stderr", &CHANNEL::read_stderr)
    .function("request_pty", &CHANNEL::request_pty)
    .function("setenv", &CHANNEL::setenv)
    .function("shell", &CHANNEL::shell)
    .function("write", &CHANNEL::write)
    .function("write_stderr", &CHANNEL::write_stderr)
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

  emscripten::class_<SFTP_HANDLE>("SFTP_HANDLE")
    .constructor<emscripten::val>()
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

  emscripten::class_<SFTP>("SFTP")
    .constructor<emscripten::val>()

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

  emscripten::class_<SESSION>("SESSION")
    .constructor<emscripten::val>()
    
    .property("socket", &SESSION::getSocket)
    .property("key", &SESSION::getKey, &SESSION::setKey)
    .property("username", &SESSION::getUsername, &SESSION::setUsername)
    .property("password", &SESSION::getPassword, &SESSION::setPassword)
    .property("send", &SESSION::getSendCallback, &SESSION::setSendCallback)
    
    .function("pushdata", &SESSION::pushdata)
    .function("userauth", &SESSION::userauth)
    .function("login", &SESSION::login)
    .function("CHANNEL", &SESSION::channel)
    .function("SFTP", &SESSION::sftp)
    ;
}
