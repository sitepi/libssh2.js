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

#include <sys/socket.h>

#include <string>

#include <iostream>
#include <queue>

#include <emscripten.h>
#include <emscripten/bind.h>


static bool libssh2_loaded = false;

class SESSION {
public:
	SESSION(emscripten::val handler) :
		handler(handler),
		startup(false),
		has_shell(false)
	{
		if(!libssh2_loaded) {
			int rc = libssh2_init(0);
		        if(rc != 0) {
        		        fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
                		return;
        		}
			fprintf(stderr, "libssh2-%s loaded\n", libssh2_version(0));
        		libssh2_loaded = true;
		}

		sock = socket(AF_INET, SOCK_STREAM, 0);

		if(connect(sock, (struct sockaddr*)this, sizeof(*this)) != 0) {
			fprintf(stderr, "failed to connect!\n");
			return;
		}

		/* Create a session instance and start it up. This will trade welcome
		 * banners, exchange keys, and setup crypto, compression, and MAC layers
		 */
		session = libssh2_session_init();
		libssh2_session_set_blocking(session, 0);

		printf("handler: %d\n", handler.as<bool>());
	}
	void setKey(std::string key) { keyContent = key; }
	std::string getKey() const { return keyContent; }

	void setUsername(std::string name) { username = name; }

	std::string getUsername() const { return username;}

	void setPassword(std::string passwd) { password = passwd; }

	std::string getPassword() const { return password; }

	int getSocket() const { return sock; }

	static std::string getStringFromInstance(const SESSION& instance) {
		return instance.key;
	}

	int handshake(emscripten::val cb);
	int _handshake();
	int login(emscripten::val cb);
	int login1(emscripten::val cb);

	void pushdata(std::string data) {
		uint8_t *p = (uint8_t*)data.c_str();
		printf("from raw: %lu\n", data.length());

		for(int i = 0; i < data.length(); i++) {
			incoming.push(*(p+i));
		}
		if(startup == false)
			handshake();
	}

	ssize_t recv(void *buffer, size_t length, int flags) 
	{
		ssize_t count = 0;
		uint8_t *p = static_cast<uint8_t*>(buffer);
		if(incoming.empty()) {
			errno = EAGAIN;
			return -errno;
		}

		while(!incoming.empty() && (count++ < length)) {
			*p++ = incoming.front();
			incoming.pop();
		}
		return count;
	}

	void clean() 
	{
		while(!incoming.empty()) {
			incoming.pop();
		}
	}

	emscripten::val getSendCallback() const { return sendcb; }
	int setSendCallback(emscripten::val cb) {
		sendcb = cb;
		return 0;
	}

private:
	int handshake()
	{
		int auth_pw = 0;
		int rc = libssh2_session_handshake(session, sock);

		if(rc) {
			if(rc != LIBSSH2_ERROR_EAGAIN) {
				int err = libssh2_session_last_errno(session);
				fprintf(stderr, "open session failed: (%d) %d\n", err, rc);
			}
		}
		else {
			fprintf(stderr, "handshake done\n");
			const char *fingerprint;
			fingerprint = libssh2_hostkey_hash(this->session, LIBSSH2_HOSTKEY_HASH_SHA1);
		        fprintf(stderr, "Fingerprint: ");
		        for(int i = 0; i < 20; i++) {
		                fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
		        }
		        fprintf(stderr, "\n");

			startup = true;
			memcpy(finger, fingerprint, 20);
		}

		return 0;
	}

public:
	emscripten::val cb = emscripten::val::null();
	emscripten::val handler = emscripten::val::null();

	LIBSSH2_SESSION *session;
	int sock;

	bool has_shell;
	
	int newchannel() {
		channel = libssh2_channel_open_session(session);
		if(!channel) {
			fprintf(stderr, "Unable to open a session\n");
		}
		else {
			fprintf(stderr, "channel ok\n");
		}
		return 0;
	}

	int request_pty() 
	{
		int rc = libssh2_channel_request_pty(channel, "vanilla");
		if(rc == LIBSSH2_ERROR_EAGAIN) {

		}
		else if (rc) {
			int err = libssh2_session_last_errno(session);
			printf("request pty failed: %d %d\n", rc, err);
		}
		else {
			fprintf(stderr, "request pty ok\n");
		}
		return 0;
	}

	int open_shell() 
	{
		int rc = libssh2_channel_shell(channel);
		if(rc == LIBSSH2_ERROR_EAGAIN) {

		}
		else if (rc) {
			int err = libssh2_session_last_errno(session);
			printf("request pty failed: %d %d\n", rc, err);
		}
		else {
			fprintf(stderr, "open shell done\n");
		}

		has_shell = true;
		return rc;
	}

	int channel_write(std::string cmd) {
		int len = libssh2_channel_write(channel, cmd.c_str(), cmd.length());
		return 0;
	}

	std::string channel_read() {
		char buf[1024];
		int len = libssh2_channel_read(channel, buf, 1024);

		if(len) {
			std::string data(buf, len);
			return data;
		}
		else {
			std::string data(buf, 1);
			return data;
		}
	}

private:
	std::string key;

	std::queue<uint8_t> incoming;
	bool startup;
	char finger[32];

	std::string keyContent;
	std::string username;
	std::string password;

	emscripten::val sendcb = emscripten::val::null();

	LIBSSH2_CHANNEL *channel;
};
