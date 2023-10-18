#include "pch.h"
#include "RedisConnection.h"
#include "RedisConnectionPool.h"

void RedisConnectionDeleter::operator() (RedisConnection* conn) const
{
	conn->FreeRedisReply();
	_pool.PutBack(conn);
}

RedisConnectionPool::RedisConnectionPool()
	: _deleter(*this)
{
}

RedisConnectionPool::~RedisConnectionPool()
{
	Clear();
}

bool RedisConnectionPool::Init(TCHAR* host, INT32 port, TCHAR* pass, INT32 max_conn)
{
	Clear();

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conversion;	
	//std::string utf8_host = tchar_to_utf8<TCHAR>::cvt(host);
	std::string utf8_host = conversion.to_bytes(host);
	hostent* HostInfo = gethostbyname(utf8_host.c_str());
	if (HostInfo == NULL) {
		printf("REDIS host를 찾을 수 없습니다. gethostbyname(%s) fail", host);
		return false;
	}

	SOCKADDR_IN SockAddr;
	memcpy(&(SockAddr.sin_addr), HostInfo->h_addr, HostInfo->h_length);
	_config.host = inet_ntoa(SockAddr.sin_addr);
	_config.port = port;
	//_config.pw = tchar_to_utf8<TCHAR>::cvt(pass);
	_config.pw = conversion.to_bytes(pass);
	_config.maxConn = std::min(max_conn, _conn_max);

	std::lock_guard<std::mutex> lck(_mutex);
	for (int i = 0; i < _config.maxConn; i++)
	{
		RedisConnection* connection = new RedisConnection();

		bool isConnected = false;
		struct timeval timeout = { 1, 500000 };

		isConnected = connection->Connect(_config.host.c_str(), _config.port, timeout);

		if (isConnected) {
			// 암호가 설정 체크
			if (!_config.pw.empty()) {
				if (connection->SendCommand("AUTH %s", _config.pw.c_str()) == false) {
					printf("REDIS: AUTH fail: %s:%d", host, port);
					delete connection;
					connection = nullptr;
					break;
				}

				connection->FreeRedisReply();
			}

			_idleConnections.push_front(connection);
			++_connectionCount;
		}
		else {
			printf("REDIS: connect fail : %s:%d", host, port);
			delete connection;
			connection = nullptr;
			break;
		}
	}

	return _idleConnections.size() == _config.maxConn;
}

void RedisConnectionPool::Clear()
{
	std::lock_guard<std::mutex> lck(_mutex);
	{
		std::list<RedisConnection*>::iterator it = _idleConnections.begin();
		for (; it != _idleConnections.end();) {
			delete (*it);
			(*it) = nullptr;
			++it;
			continue;
		}
		_idleConnections.clear();
	}

	{
		std::list<RedisConnection*>::iterator it = _activeConnections.begin();
		for (; it != _activeConnections.end();) {
			delete (*it);
			(*it) = nullptr;
			++it;
			continue;
		}
		_activeConnections.clear();
	}

	_connectionCount = 0;
	_config.Clear();
}

std::shared_ptr<RedisConnection> RedisConnectionPool::Get()
{
	std::lock_guard<std::mutex> lck(_mutex);

	RedisConnection* connection = nullptr;
	if (_idleConnections.empty()) {
		if (_connectionCount < _config.maxConn) {
			connection = new RedisConnection();

			bool isConnected = false;
			struct timeval timeout = { 1, 500000 };
			isConnected = connection->Connect(_config.host.c_str(), _config.port, timeout);

			if (isConnected) {
				// 암호가 설정 체크
				if (!_config.pw.empty()) {
					if (connection->SendCommand("AUTH %s", _config.pw.c_str()) == false) {
						printf("REDIS: AUTH fail");
						delete connection;
						connection = nullptr;
						return nullptr;
					}

					connection->FreeRedisReply();
				}

				++_connectionCount;
			}
			else {
				printf("REDIS: connect fail");
				delete connection;
				connection = nullptr;
				return nullptr;
			}
		}
		else {
			printf("REDIS: reached to connection limit");
			return nullptr;
		}
	}
	else {
		connection = _idleConnections.front();
		_idleConnections.pop_front();
	}
	assert(connection);

	if (connection->SendCommand("PING") == false) {

		delete connection;
		connection = nullptr;
		--_connectionCount;
		return nullptr;
	}

	connection->FreeRedisReply();

	_activeConnections.push_front(connection);
	if (_config.maxConn < _activeConnections.size())
	{
		_config.maxConn = (int)_activeConnections.size();
	}

	return std::shared_ptr<RedisConnection>(connection, _deleter);
}

void RedisConnectionPool::PutBack(RedisConnection* connection)
{
	std::lock_guard<std::mutex> lck(_mutex);

	std::list<RedisConnection*>::iterator it = std::find(_activeConnections.begin(), _activeConnections.end(), connection);
	if (it != _activeConnections.end()) {
		if (connection->IsConnected()) {
			_idleConnections.push_front(connection);
		}
		else {
			--_connectionCount;
		}

		_activeConnections.erase(it);
	}
	else {
		assert(!"PutBack Failed");
	}
}