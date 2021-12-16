/*
 *
 */

#ifndef _SSH2_SFTP_H_
#define _SSH2_SFTP_H_

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <emscripten.h>
#include <emscripten/bind.h>

class SFTP {
public:
	SFTP(emscripten::val handle)
	{
		sftp = libssh2_sftp_init(session);
	}

	SFTP(LIBSSH2_SESSION *sess) :
		session(sess)
	{
	}

	int close()
	{
		return libssh2_sftp_close(handle);
	}

	int closedir()
	{
		return libssh2_sftp_closedir(handle);
	}

	LIBSSH2_SFTP_ATTRIBUTES fsetstat()
	{
		LIBSSH2_SFTP_ATTRIBUTES attrs;
		memset(&attrs, '\0', sizeof(attrs));

		if(libssh2_sftp_fsetstat(handle, &attrs)) {
		}
		return attrs;
	}

	LIBSSH2_SFTP_ATTRIBUTES fstat(int setstat = 0)
	{
		LIBSSH2_SFTP_ATTRIBUTES attrs;
		memset(&attrs, '\0', sizeof(attrs));

		if(libssh2_sftp_fstat_ex(handle, &attrs, setstat)) {
		}
		return attrs;
	}

	LIBSSH2_SFTP_STATVFS fstatvfs() 
	{
		LIBSSH2_SFTP_STATVFS st;
		memset(&st, '\0', sizeof(st));

		if(libssh2_sftp_fstatvfs(handle,  &st)) {
		}

		return st;
	}

	int fsync()
	{
		return libssh2_sftp_fsync(handle);
	}

	int init() 
	{
		sftp = libssh2_sftp_init(session);
		return 0;
	}

	int last_error()
	{
		return libssh2_sftp_last_error(sftp);
	}

	LIBSSH2_SFTP_ATTRIBUTES lstat(std::string path)
	{
		LIBSSH2_SFTP_ATTRIBUTES attrs;
		memset(&attrs, '\0', sizeof(attrs));

		if(libssh2_sftp_lstat(sftp, path.c_str(), &attrs)) {
		}
		return attrs;
	}

	int mkdir(std::string path, long mode) 
	{
		return libssh2_sftp_mkdir_ex(sftp,
				path.c_str(), path.length(), mode);
	}

	int open(std::string path, unsigned long flags, long mode, int type) 
	{
		handle = libssh2_sftp_open_ex(sftp, 
				path.c_str(), 
				path.length(),
				flags, mode, type);
		return (handle) ? 0 : -1;
	}

	int opendir(std::string path) 
	{
		handle = libssh2_sftp_opendir(sftp, path.c_str());
		return (handle) ? 0 : -1;
	}

	int read() 
	{
		return libssh2_sftp_read(handle, NULL, 0);
	}

	int readdir() 
	{
		return libssh2_sftp_readdir(handle, NULL, 0, NULL);
	}

	int readdir_ex() 
	{
		return libssh2_sftp_readdir_ex(handle, NULL, 0, 
				NULL, 0, NULL);
	}

	int readlink(std::string path, std::string target) {
		return libssh2_sftp_readlink(sftp, 
				path.c_str(), NULL, 0);
	}
	int realpath() {
		return libssh2_sftp_realpath(sftp, NULL, NULL, 0);
	}

	int rename(std::string source, std::string dest, long flags) {
		return libssh2_sftp_rename_ex(sftp,
				source.c_str(),
				source.length(),
				dest.c_str(),
				dest.length(),
				flags);
	}

	int rewind() {
		libssh2_sftp_rewind(handle);
		return 0;
	}

	int rmdir(std::string path)
	{
		return libssh2_sftp_rmdir_ex(sftp, path.c_str(), path.length());
	}

	int seek(size_t offset) 
	{
		libssh2_sftp_seek(handle, offset);
		return 0;
	}

	int seek64(libssh2_uint64_t offset)
	{
		libssh2_sftp_seek64(handle, offset);
		return 0;
	}

	int setstat(std::string path)
	{
		return libssh2_sftp_setstat(sftp, path.c_str(), NULL);
	}

	int shutdown()
	{
		return libssh2_sftp_shutdown(sftp);
	}

	LIBSSH2_SFTP_ATTRIBUTES stat(std::string path, 
			int type = LIBSSH2_SFTP_STAT)
	{
		LIBSSH2_SFTP_ATTRIBUTES attrs;
		memset(&attrs, '\0', sizeof(attrs));

		if(libssh2_sftp_stat_ex(sftp,
				path.c_str(), 
				path.length(), type, &attrs)) {
		}
		return attrs;
	}

	LIBSSH2_SFTP_STATVFS statvfs(std::string path)
	{
		LIBSSH2_SFTP_STATVFS st;
		memset(&st, '\0', sizeof(st));

		if(libssh2_sftp_statvfs(sftp,
				path.c_str(),
				path.length(), &st)) {
		}

		return st;
	}

	int symlink(std::string orig, std::string dest, 
					int type = LIBSSH2_SFTP_SYMLINK)
	{
		char buf[1024];
		return libssh2_sftp_symlink_ex(sftp, 
				orig.c_str(),
				orig.length(),
				buf,
				1024,
				type);

	}

	int tell() {
		return libssh2_sftp_tell(handle);
	}

	int tell64() {
		return libssh2_sftp_tell64(handle);
	}

	int unlink(std::string fil)
	{
		return libssh2_sftp_unlink_ex(sftp, fil.c_str(), fil.length());
	}

	int write(std::string buff)
	{
		return libssh2_sftp_write(handle, buff.c_str(), buff.length());
	}

#if 0
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
	emscripten::val & statvfs_object(emscripten::val &v,
			const LIBSSH2_SFTP_STATVFS *st) {
		v.set("bsize", st->f_bsize);
		v.set("frsize", st->f_frsize);
		v.set("blocks", st->f_blocks);
		v.set("bfree", st->f_bfree);
		v.set("bavail", st->f_bavail);
		v.set("files", st->f_files);
		v.set("ffree", st->f_ffree);
		v.set("favail", st->f_favail);
		v.set("fsid", st->f_fsid);
		v.set("flag", st->f_flag);
		v.set("namemax", st->f_namemax);

		return v;
	}
#endif
private:
	LIBSSH2_SFTP 	    *sftp;
	LIBSSH2_SFTP_HANDLE *handle;
	LIBSSH2_SESSION     *session;
};

#endif /* ~_SSH2_SFTP_H_ */

