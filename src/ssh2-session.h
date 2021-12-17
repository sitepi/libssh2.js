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

class SESSION {
public:
	SESSION(emscripten::val v) :
		has_error(false),
		has_opened(false)
	{
		fd = socket(AF_INET, SOCK_STREAM, 0);

		if(connect(fd, (struct sockaddr*)this, sizeof(*this))!= 0) {
			fprintf(stderr, "failed to connect!\n");
			return;
		}

		session = libssh2_session_init();
		libssh2_session_set_blocking(session, 0);
		
#if 0
		if(v["onmessage"].as<bool>()) {
			v.call<void>("onmessage", [](emscripten::val msg) {
			});
		}
#endif
	}

	void setKey(std::string key) { keyContent = key; }
	std::string getKey() const { return keyContent; }
	void setUser(std::string v) { user = v; }
	std::string getUser() const { return user; }
	void setPwd(std::string v) { passwd = v; }
	std::string getPwd() const  { return passwd; }
	emscripten::val getSendCb() const { return send; }
	void setSendCb(emscripten::val cb)  { send = cb; }
	
	void pushdata(std::string data)
	{
		uint8_t *p = (uint8_t*)data.c_str();
		// printf("from raw: %lu\n", data.length());

		for(int i = 0; i < data.length(); i++) {
			incoming.push(*(p+i));
		}
		if(has_error) {
			fprintf(stderr, "session has_error\n");
			//closesocket(fd);
		}
		else if(!has_opened)
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

private:
	int handshake();

public:
	int login();
	int userauth();

	CHANNEL channel();
	SFTP    sftp();

	emscripten::val send = emscripten::val::null();

private:
	int  fd;
	bool has_opened;
	bool has_error;
	LIBSSH2_SESSION *session;

	std::queue<uint8_t> incoming;

	std::string keyContent;
	std::string user;
	std::string passwd;

	std::string fingerprint;
	std::string methods;

	emscripten::val onopen    = emscripten::val::null();
	emscripten::val onerror   = emscripten::val::null();
	emscripten::val onclose   = emscripten::val::null();
};

#endif /* ~_SSH2_SESSION_H_ */
