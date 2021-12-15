/*
 * Sample showing how to do SSH2 connect.
 *
 * The sample code has default values for host name, user name, password
 * and path to copy, but you can specify them on the command line like:
 *
 * "ssh2 host user password [-p|-i|-k]"
 */

#include "ssh2-session.h"

using namespace emscripten;
//using namespace std;

int SESSION::login1(emscripten::val cb) 
{
	/* At this point we havn't authenticated. The first thing to do is check
	 * the hostkey's fingerprint against our known hosts Your app may have it
	 * hard coded, may go to a file, may present it to the user, that's your
	 * call
	 */
	int auth_pw = 0;
	char *userauthlist;

	/* check what authentication methods are available */
	userauthlist = libssh2_userauth_list(session, username.c_str(), username.length());
	fprintf(stderr, "Authentication methods: %s\n", userauthlist);
	if(strstr(userauthlist, "password") != NULL) {
		auth_pw |= 1;
	}
	if(strstr(userauthlist, "keyboard-interactive") != NULL) {
		auth_pw |= 2;
	}
	if(strstr(userauthlist, "publickey") != NULL) {
		auth_pw |= 4;
	}
	return 0;
}

int SESSION::login(emscripten::val cb)
{
//	if(auth_pw & 1) {
		/* We could authenticate via password */
		if(libssh2_userauth_password(session, username.c_str(), password.c_str())) {
			fprintf(stderr, "\tAuthentication by password failed!\n");
		}
		else {
			fprintf(stderr, "\tAuthentication by password succeeded.\n");
		}
//	}
	return 0;
}

// Binding code
EMSCRIPTEN_BINDINGS(libssh2_session) {
  class_<SESSION>("SESSION")
    .constructor<emscripten::val>()
    .property("socket", &SESSION::getSocket)
    .property("key", &SESSION::getKey, &SESSION::setKey)
    .property("username", &SESSION::getUsername, &SESSION::setUsername)
    .property("password", &SESSION::getPassword, &SESSION::setPassword)
    .property("send", &SESSION::getSendCallback,&SESSION::setSendCallback)
    .function("pushdata", &SESSION::pushdata)
    .function("login", &SESSION::login)
    .function("login1", &SESSION::login1)
    .function("channel", &SESSION::newchannel)
    .function("pty", &SESSION::request_pty)
    .function("shell", &SESSION::open_shell)
    .function("ch_write", &SESSION::channel_write)
    .function("ch_read", &SESSION::channel_read)
//    .class_function("getStringFromInstance", &SESSION::getStringFromInstance)
    ;
}

/*
int main()
{
	int rc = libssh2_init(0);
	if(rc != 0) {
		fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
		return 1;
	}
	//	emscripten_set_main_loop(socket_walk, 0, 1);
	ssh2_init = true;

	return 0;
}
*/
