/*
 * Sample showing how to do SSH2 connect.
 *
 * The sample code has default values for host name, user name, password
 * and path to copy, but you can specify them on the command line like:
 *
 * "ssh2 host user password [-p|-i|-k]"
 */

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <emscripten.h>
#include <emscripten/bind.h>

class SFTP {
public:
	SFTP(emscripten::val handler)
	{

	}

	int close() {
		return libssh2_sftp_close(handle);
	}
	int closedir() {
		return libssh2_sftp_closedir(handle);
	}

	int fsetstat() {
		return libssh2_sftp_fsetstat(handle, NULL);
	}

	int fstat() {
		return libssh2_sftp_fstat(handle, NULL);
	}

	int fstat_ex() {
		return libssh2_sftp_fstat(handle, NULL);
	}

	int fstatvfs() {
		//return libssh2_sftp_statvfs(handle, NULL, 0, NULL);
		return 0;
	}

	int fsync() {
		return libssh2_sftp_fsync(handle);
	}

	int get_channel() {
		return 0;
	}

	int init() {
		sftp = libssh2_sftp_init(session);
		return 0;
	}
	int last_error() {
		return libssh2_sftp_last_error(sftp);
	}

	int lstat() {
		return libssh2_sftp_lstat(sftp, NULL, NULL);
	}

	int mkdir() {
		return libssh2_sftp_mkdir(sftp, NULL, 0);
	}

	int mkdir_ex() {
		return libssh2_sftp_mkdir_ex(sftp, NULL, 0, 0);
	}

	int open() {
		handle = libssh2_sftp_open(sftp, NULL, 0, 0);
		return 0;
	}

	int open_ex() {
		handle = libssh2_sftp_open_ex(sftp, NULL, 0,0,0,0);
		return 0;
	}

	int opendir() {
		handle = libssh2_sftp_opendir(sftp, NULL);
		return 0;
	}

	int read() {
		return libssh2_sftp_read(handle, NULL, 0);
	}

	int readdir() {
		return libssh2_sftp_readdir(handle, NULL, 0, NULL);
	}

	int readdir_ex() {
		return libssh2_sftp_readdir_ex(handle, NULL, 0, 
				NULL, 0, NULL);
	}

	int readlink() {
		return libssh2_sftp_readlink(sftp, NULL, NULL, 0);
	}
	int realpath() {
		return libssh2_sftp_realpath(sftp, NULL, NULL, 0);
	}

	int rename() {
		return libssh2_sftp_rename(sftp, NULL, NULL);
	}

	int rename_ex() {
		return libssh2_sftp_rename_ex(sftp, NULL, 0, NULL, 0, 0);
	}

	int rewind() {
		libssh2_sftp_rewind(handle);
		return 0;
	}

	int rmdir() {
		return libssh2_sftp_rmdir(sftp, NULL);
	}

	int rmdir_ex() {
		return libssh2_sftp_rmdir_ex(sftp, NULL, 0);
	}

	int seek() {
		libssh2_sftp_seek(handle, 0);
		return 0;
	}

	int seek64() {
		libssh2_sftp_seek64(handle, 0);
		return 0;
	}

	int setstat() {
		return libssh2_sftp_setstat(sftp, NULL, NULL);
	}

	int shutdown() {
		return libssh2_sftp_shutdown(sftp);
	}

	int stat() {
		return libssh2_sftp_fstat(handle, NULL);
	}

	int stat_ex() {
		return libssh2_sftp_stat_ex(sftp, NULL, 0, 0, NULL);
	}

	int statvfs() {
		return libssh2_sftp_statvfs(sftp, NULL, 0, NULL);
	}

	int symlink() {
		return libssh2_sftp_symlink(sftp, NULL, NULL);
	}

	int symlink_ex() {
		return libssh2_sftp_symlink_ex(sftp, NULL, 0, NULL, 0, 0);
	}

	int tell() {
		return libssh2_sftp_tell(handle);
	}

	int tell64() {
		return libssh2_sftp_tell64(handle);
	}

	int unlink() {
		return libssh2_sftp_unlink(sftp, NULL);
	}

	int unlink_ex() {
		return libssh2_sftp_unlink_ex(sftp, NULL, 0);
	}
	int write() {
		return libssh2_sftp_write(handle, NULL, 0);
	}


private:
	emscripten::val sendcb = emscripten::val::null();

	LIBSSH2_SFTP 	    *sftp;
	LIBSSH2_SFTP_HANDLE *handle;
	LIBSSH2_SESSION     *session;
};
