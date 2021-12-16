/*
 *
 */
#ifndef _SSH2_SESSION_H_
#define _SSH2_SESSION_H_

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <sys/socket.h>

#include <string>

#include <iostream>
#include <queue>

#include <emscripten/bind.h>

#include "ssh2-channel.h"
#include "ssh2-sftp.h"

using namespace std;

class SESSION {
public:
	SESSION(emscripten::val handle) :
		handle(handle),
		startup(false)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);

		if(connect(sock, (struct sockaddr*)this, sizeof(*this)) != 0) {
			fprintf(stderr, "failed to connect!\n");
			return;
		}

		session = libssh2_session_init();
		libssh2_session_set_blocking(session, 0);

		printf("handle: %d\n", handle.as<bool>());

		printf("ondata: %d\n", handle["ondata"].as<bool>());
#if 0
		if(handle["onmessage"].as<bool>()) {
			handle.call<void>("onmessage", [](emscripten::val msg) {

			});
		}
#endif
	}
	void setKey(std::string key) 
	{
		keyContent = key; 
	}

	std::string getKey() const 
	{
		return keyContent; 
	}

	void setUsername(std::string name) 
	{
		username = name;
	}

	std::string getUsername() const 
	{
		return username;
	}

	void setPassword(std::string passwd)
	{
		password = passwd;
	}

	std::string getPassword() const 
	{
		return password;
	}

	int getSocket() const {
		return sock;
	}

	void pushdata(std::string data)
	{
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

	emscripten::val getSendCallback() const 
	{
		return sendcb;
	}
	int setSendCallback(emscripten::val cb) {
		sendcb = cb;
		return 0;
	}

private:
	int handshake()
	{
		int auth_pw = 0;
		int rc = libssh2_session_handshake(session, sock);

		if(rc == LIBSSH2_ERROR_EAGAIN) {
			;
		}
		else if(rc) {
			int err = libssh2_session_last_errno(session);
			fprintf(stderr, "open session failed: (%d) %d\n", err, rc);
		}
		else {
			fprintf(stderr, "handshake done\n");
			const char *hostkey;
			hostkey = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
		        fprintf(stderr, "Fingerprint: ");
		        for(int i = 0; i < 20; i++) {
		                fprintf(stderr, "%02X ", (unsigned char)hostkey[i]);
		        }
		        fprintf(stderr, "\n");

			startup = true;
			memcpy(fingerprint, hostkey, 20);
		}

		return 0;
	}

public:
	CHANNEL channel();
	SFTP sftp();

	int login();
	int userauth();

private:
	LIBSSH2_SESSION *session;
	int sock;

	std::queue<uint8_t> incoming;
	bool startup;
	char fingerprint[32];

	std::string keyContent;
	std::string username;
	std::string password;

	emscripten::val sendcb = emscripten::val::null();

	//emscripten::val cb = emscripten::val::null();
	emscripten::val handle = emscripten::val::null();
};

#endif /* ~_SSH2_SESSION_H_ */

