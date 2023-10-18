#pragma once
class RedisConnection;
class RedisConnectionPool;

struct RedisConfig
{
	std::string host;
	unsigned short port;
	std::string pw;
	int maxConn;

	void Clear()
	{
		host.clear();
		port = 0;
		pw.clear();
		maxConn = 0;
	}
};

//	이름은 deleter지만 shared_ptr의 deleter로 사용될 뿐 실제론 풀에 리턴하는 역할만 한다
class RedisConnectionDeleter
{
public:
	RedisConnectionDeleter(RedisConnectionPool& pool) : _pool(pool) {}
	void operator() (RedisConnection* conn) const;

	RedisConnectionDeleter(const RedisConnectionDeleter& r) = default;
	RedisConnectionDeleter& operator=(const RedisConnectionDeleter& r) = delete;

private:
	RedisConnectionPool& _pool;
};

class RedisConnectionPool
{
	friend class RedisConnectionDeleter;
public:

	RedisConnectionPool();
	~RedisConnectionPool();

	bool Init(TCHAR* host, INT32 port, TCHAR* pass, INT32 max_conn);
	std::shared_ptr<RedisConnection> Get();

private:
	void Clear();
	void PutBack(RedisConnection* connection);

private:
	std::mutex _mutex;
	size_t _connectionCount;
	RedisConfig _config;

	constexpr static int _conn_max = 10;
	std::list<RedisConnection*> _idleConnections;
	std::list<RedisConnection*> _activeConnections;
	RedisConnectionDeleter _deleter;
};