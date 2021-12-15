/*
 * Sample showing how to do SSH2 connect.
 *
 * The sample code has default values for host name, user name, password
 * and path to copy, but you can specify them on the command line like:
 *
 * "ssh2 host user password [-p|-i|-k]"
 */

#include "ssh2-sftp.h"

using namespace emscripten;

// Binding code
EMSCRIPTEN_BINDINGS(libssh2_sftp) {
  class_<SFTP>("SFTP")
    .constructor<emscripten::val>()

    .function("close", &SFTP::close)
//    .function("close_handle", &SFTP::close_handle)
    .function("closedir", &SFTP::closedir)
    .function("fsetstat", &SFTP::fsetstat)
    .function("fstat", &SFTP::fstat)
    .function("fstat_ex", &SFTP::fstat_ex)
    .function("fstatvfs", &SFTP::fstatvfs)
    .function("fsync", &SFTP::fsync)
    .function("get_channel", &SFTP::get_channel)
    .function("init", &SFTP::init)
    .function("last_error", &SFTP::last_error)
    .function("lstat", &SFTP::lstat)
    .function("mkdir", &SFTP::mkdir)
    .function("mkdir_ex", &SFTP::mkdir_ex)
    .function("open", &SFTP::open)
    .function("open_ex", &SFTP::open_ex)
    .function("opendir", &SFTP::opendir)
    .function("read", &SFTP::read)
    .function("readdir", &SFTP::readdir)
    .function("readdir_ex", &SFTP::readdir_ex)
    .function("readlink", &SFTP::readlink)
    .function("realpath", &SFTP::realpath)
    .function("rename", &SFTP::rename)
    .function("rename_ex", &SFTP::rename_ex)
    .function("rewind", &SFTP::rewind)
    .function("rmdir", &SFTP::rmdir)
    .function("rmdir_ex", &SFTP::rmdir_ex)
    .function("seek", &SFTP::seek)
    .function("seek64", &SFTP::seek64)
    .function("setstat", &SFTP::setstat)
    .function("shutdown", &SFTP::shutdown)
    .function("stat", &SFTP::stat)
    .function("stat_ex", &SFTP::stat_ex)
    .function("statvfs", &SFTP::statvfs)
    .function("symlink", &SFTP::symlink)
    .function("symlink_ex", &SFTP::symlink_ex)
    .function("tell", &SFTP::tell)
    .function("tell64", &SFTP::tell64)
    .function("unlink", &SFTP::unlink)
    .function("unlink_ex", &SFTP::unlink_ex)
    .function("write", &SFTP::write)

    ;
}

