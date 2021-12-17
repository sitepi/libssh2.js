/*
 *
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
		//
		fprintf(stderr, "SFTP CANNOT CREATE DIRECTLY!\n");
	}

	SFTP(LIBSSH2_SESSION *sess, LIBSSH2_SFTP *sf) :
			session(sess),
			sftp(sf)
	{
		if(sftp) {
			fprintf(stderr, "SFTP ok\n");
			active = true;
		}
	}

	LIBSSH2_SFTP_ATTRIBUTES lstat(std::string path)
	{
		memset(&attrs, '\0', sizeof(attrs));

		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return attrs;
		}

		if(libssh2_sftp_lstat(sftp, path.c_str(), &attrs)) {
		
		}
		return attrs;
	}

	int mkdir(std::string path, long mode) 
	{
		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return 0;
		}

		return libssh2_sftp_mkdir_ex(sftp,
				path.c_str(), path.length(), mode);
	}

	SFTP_HANDLE open(std::string path, 
				unsigned long flags, long mode, int type) 
	{
		LIBSSH2_SFTP_HANDLE *h = NULL;
		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return SFTP_HANDLE(sftp, h);
		}

		h = libssh2_sftp_open_ex(sftp, 
					path.c_str(), 
					path.length(),
					flags, mode, type);
		if(!h) {
			//TODO: 
		}

		return SFTP_HANDLE(sftp, h);
	}

	SFTP_HANDLE opendir(std::string path) 
	{
		LIBSSH2_SFTP_HANDLE *h = NULL;
		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return SFTP_HANDLE(sftp, h);
		}

		h = libssh2_sftp_opendir(sftp, path.c_str());
		if(!h) {
			//TODO: 
		}
		return SFTP_HANDLE(sftp, h);
	}

	std::string readlink(std::string path) 
	{
		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return nodata;
		}

		int n = libssh2_sftp_readlink(sftp, 
					path.c_str(), buffer, BUFF_LEN);
		return (n) ? std::string(buffer, n) : nodata;
	}

	int unlink(std::string path)
	{
		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return 0;
		}

		return libssh2_sftp_unlink_ex(sftp,
				path.c_str(), path.length());
	}

	std::string realpath(std::string path)
	{
		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return nodata;
		}

		int n = libssh2_sftp_realpath(sftp, 
					path.c_str(), buffer, BUFF_LEN);
		return (n) ? std::string(buffer, n) : nodata;
	}

	int rename(std::string source, std::string dest, long flags)
	{
		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return 0;
		}

		return libssh2_sftp_rename_ex(sftp,
					source.c_str(),
					source.length(),
					dest.c_str(),
					dest.length(),
					flags);
	}

	int rmdir(std::string path)
	{
		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return 0;
		}

		return libssh2_sftp_rmdir_ex(sftp, path.c_str(), path.length());
	}

	int setstat(std::string path)
	{
		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return 0;
		}

		return libssh2_sftp_setstat(sftp, path.c_str(), NULL);
	}

	int shutdown()
	{
		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return 0;
		}

		int rc = libssh2_sftp_shutdown(sftp);

		return rc;
	}

	emscripten::val stat(std::string path, 
				int type = LIBSSH2_SFTP_STAT)
	{
		emscripten::val v = emscripten::val::object();

		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return v;
		}

		memset(&attrs, '\0', sizeof(attrs));
		if(libssh2_sftp_stat_ex(sftp,
					path.c_str(), 
					path.length(),
					type, &attrs)) {
			return attrs_object(v, &attrs);
		}
		return v;
	}

	LIBSSH2_SFTP_STATVFS statvfs(std::string path)
	{
		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return st;
		}

		memset(&attrs, '\0', sizeof(attrs));
		if(libssh2_sftp_statvfs(sftp,
					path.c_str(),
					path.length(),
					&st)) {
		}

		return st;
	}

	std::string symlink(std::string orig, std::string dest, 
				int type = LIBSSH2_SFTP_SYMLINK)
	{
		if(!sftp) {
			fprintf(stderr, "sftp error\n");
			return nodata;
		}

		ssize_t n= libssh2_sftp_symlink_ex(sftp, 
					orig.c_str(),
					orig.length(),
					buffer,
					BUFF_LEN,
					type);
		return (n) ? std::string(buffer, n) : nodata;

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
	LIBSSH2_SESSION     *session;

	LIBSSH2_SFTP_ATTRIBUTES attrs;
	LIBSSH2_SFTP_STATVFS    st;

	char buffer[BUFF_LEN];
	std::string nodata;

	bool active = false;
};

#endif /* ~_SSH2_SFTP_H_ */

