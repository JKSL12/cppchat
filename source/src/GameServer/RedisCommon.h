#pragma once
#include "RedisConnectionPool.h"

class RedisCommand;

class RedisCommon
{
public:
	virtual ~RedisCommon();

	void Init(TCHAR* const host, const INT32 port, TCHAR* const pass, const INT32 max_conn, const INT32 thread_count);
	std::shared_ptr<RedisConnection> GetConnection() { return mConnectionPool.Get(); }
	bool PostCommand(std::shared_ptr<RedisCommand> command);

	bool ConnectionTest();

private:
	static constexpr int MAX_THROUGHPUT = 100;
	static constexpr int IDLE_TIME = 1000;

	void PumpingThreadFunc();

	RedisConnectionPool mConnectionPool;
	boost::thread_group pool;
	volatile bool run_pumping = false;
	std::list<std::shared_ptr<RedisCommand>> q;
	std::mutex m;	//for q

public:
	static RedisCommon* Instance();
private:
	static RedisCommon* _instance;
};

