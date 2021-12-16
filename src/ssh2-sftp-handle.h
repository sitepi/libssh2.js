/*
 *
 */

#ifndef _SSH2_SFTP_HANDLE_H_
#define _SSH2_SFTP_HANDLE_H_

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <emscripten.h>
#include <emscripten/bind.h>

class SFTP_HANDLE {
public:
	SFTP_HANDLE(emscripten::val handle)
	{
		sftp = libssh2_sftp_init(session);
	}

	SFTP_HANDLE(LIBSSH2_SFTP *sf, LIBSSH2_SFTP_HANDLE *hand) :
		sftp(sf),
		handle(hand)
	{
		if(handle) {
			fprintf(stderr, "SFTP_HANDLE ok\n");
		}
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

	int rewind() {
		libssh2_sftp_rewind(handle);
		return 0;
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
};

#endif /* ~_SSH2_SFTP_HANDLE_H_ */

