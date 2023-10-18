#include <stdio.h>
#include "RedisConnection.h"
#include <sstream>

RedisConnection::RedisConnection()
{
	//InitializeCriticalSection(&_cs);
	Init();
}


RedisConnection::~RedisConnection()
{
	Clear();
	//DeleteCriticalSection(&_cs);
}

void RedisConnection::Init()
{
	_context = NULL;
	_reply = NULL;
	_connected = false;
}

void RedisConnection::Clear()
{
	_connected = false;
	if (_context != NULL) {
		redisFree(_context);
	}
	_context = NULL;

	FreeRedisReply();
}

void RedisConnection::FreeRedisReply()
{
	if (_reply != NULL)
		freeReplyObject(_reply);

	_reply = NULL;
}

bool RedisConnection::Connect(const char* ip, int port, timeval timeout)
{
	//ScopedLock sl(&_cs);

	if (_context != NULL)
		return false;

	_context = redisConnectWithTimeout(ip, port, timeout);
	if (_context->err) {
		return false;
	}

	_connected = true;
	return true;
}

bool RedisConnection::Disconnect()
{
	//ScopedLock sl(&_cs);

	Clear();
	return true;
}

bool RedisConnection::SendCommand(const char* format, ...)
{
	//ScopedLock sl(&_cs);

	if (_context == NULL)
		return false;

	FreeRedisReply();

	va_list ap;
	va_start(ap, format);
	_reply = (redisReply*)redisvCommand(_context, format, (va_list)ap);
	va_end(ap);

	if (_reply == NULL) {
		Clear();
		return false;
	}

	if (_reply->type == REDIS_REPLY_ERROR) {
		FreeRedisReply();
		return false;
	}

	//if (_reply->type == REDIS_REPLY_NIL)
		//return false;

	return true;
}
