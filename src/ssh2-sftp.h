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

	SFTP(LIBSSH2_SESSION *sess, LIBSSH2_SFTP *sf) :
		session(sess),
		sftp(sf)
	{
		if(sftp) {
			fprintf(stderr, "SFTP ok\n");
		}
	}
#if 0
	emscripten::val lstat(std::string path) 
	{
		LIBSSH2_SFTP_ATTRIBUTES attrs;
		emscripten::val v = emscripten::val::object();

                memset(&attrs, '\0', sizeof(attrs));
		if(libssh2_sftp_lstat(sftp, path.c_str(), &attrs)) {
			return attrs_object(v, &attrs);
		}

		return v;

	}
#endif

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


	int rmdir(std::string path)
	{
		return libssh2_sftp_rmdir_ex(sftp, path.c_str(), path.length());
	}

	int setstat(std::string path)
	{
		return libssh2_sftp_setstat(sftp, path.c_str(), NULL);
	}

	int shutdown()
	{
		return libssh2_sftp_shutdown(sftp);
	}

	emscripten::val stat(std::string path, 
			int type = LIBSSH2_SFTP_STAT)
	{
		LIBSSH2_SFTP_ATTRIBUTES attrs;
		memset(&attrs, '\0', sizeof(attrs));
		emscripten::val v = emscripten::val::object();

		if(libssh2_sftp_stat_ex(sftp,
				path.c_str(), 
				path.length(), type, &attrs)) {
			return attrs_object(v, &attrs);
		}
		return v;
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

	int unlink(std::string fil)
	{
		return libssh2_sftp_unlink_ex(sftp, fil.c_str(), fil.length());
	}

private:
	emscripten::val attrs_object(emscripten::val &v, 
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
	LIBSSH2_SFTP 	    *sftp;
	LIBSSH2_SFTP_HANDLE *handle;
	LIBSSH2_SESSION     *session;
};

#endif /* ~_SSH2_SFTP_H_ */

