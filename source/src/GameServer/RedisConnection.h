#pragma once

#include <WinSock2.h>
#include "hiredis.h"

class RedisConnection
{
public:
	RedisConnection();
	~RedisConnection();

	void Init();
	void Clear();
	void FreeRedisReply();

	bool IsConnected() { return _connected; }

	bool Connect(const char* ip, int port, timeval timeout);
	bool Disconnect();

	bool SendCommand(const char* format, ...);

	redisReply* GetRedisReply() { return _reply; }

	redisContext* GetContext() { return _context; }
private:
	//CRITICAL_SECTION _cs;
	bool _connected;

	redisContext* _context;
	redisReply* _reply;
};

