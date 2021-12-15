/*
 * Sample showing how to do SSH2 connect.
 *
 * The sample code has default values for host name, user name, password
 * and path to copy, but you can specify them on the command line like:
 *
 * "ssh2 host user password [-p|-i|-k]"
 */

#include "ssh2-channel.h"

using namespace emscripten;
//using namespace std;

// Binding code
EMSCRIPTEN_BINDINGS(libssh2_channel) {
  class_<CHANNEL>("CHANNEL")
    .constructor<emscripten::val>()
    //.property("exit_singal", &CHANNEL::get_exit_signal)
    //.property("exit_status", &CHANNEL::get_exit_status)

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

//    .class_function("getStringFromInstance", &CHANNEL::getStringFromInstance)
    ;
}
