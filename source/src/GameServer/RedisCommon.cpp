#include "pch.h"
#include "RedisCommon.h"
#include "RedisConnection.h"

RedisCommon* RedisCommon::_instance = NULL;

RedisCommon* RedisCommon::Instance()
{
	if (!_instance) {
		_instance = new RedisCommon();
	}

	return _instance;
}

RedisCommon::~RedisCommon()
{
	run_pumping = false;
	pool.join_all();
}

void RedisCommon::Init(TCHAR* const host, const INT32 port, TCHAR* const pass, const INT32 max_conn, const INT32 thread_count)
{
	mConnectionPool.Init(host, port, pass, max_conn);

	run_pumping = true;
	//worker
	/*for (int i = 0; i < thread_count; ++i)
		pool.create_thread(std::bind(&RedisCommon::PumpingThreadFunc, this));*/
}

bool RedisCommon::PostCommand(std::shared_ptr<RedisCommand> command)
{
	std::lock_guard<std::mutex> l(m);

	q.push_back(command);
	return true;
}

bool RedisCommon::ConnectionTest()
{
	auto con = GetConnection();
	if (!con) {
		printf("ERROR: redis failed\n");
		return false;
	}

	time_t cur;
	time(&cur);

	con->SendCommand("HSET TEST 1 %lld", cur);

	return true;
}

void RedisCommon::PumpingThreadFunc()
{
	//std::list<std::shared_ptr<RedisCommand>> localq;

	//while (run_pumping)
	//{
	//	if (q.empty() && localq.empty())
	//	{
	//		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	//		continue;
	//	}

	//	{
	//		if (!m.try_lock())
	//		{
	//			std::this_thread::sleep_for(std::chrono::milliseconds(10));
	//			continue;
	//		}

	//		if (!q.empty())
	//		{
	//			size_t qsize = q.size();
	//			if (qsize >= RedisCommon::MAX_THROUGHPUT)
	//				qsize = RedisCommon::MAX_THROUGHPUT;

	//			auto it = q.begin();
	//			std::advance(it, qsize);
	//			localq.splice(localq.end(), q, q.begin(), it);

	//			m.unlock();
	//		}
	//		else
	//		{
	//			m.unlock();
	//			std::this_thread::sleep_for(std::chrono::milliseconds(10));
	//			continue;
	//		}
	//	}

	//	auto itr = localq.begin();

	//	for (auto itrend = localq.end(); itr != itrend; ++itr)
	//	{
	//		if (!(*itr)->ProcessRequest())
	//		{
	//			break;
	//		}

	//		/*if ((*itr)->GetRequireProcessResponse())
	//		{
	//			std::lock_guard<std::mutex> lock(responseQueueMutex);

	//			responseQueue.push_back(*itr);
	//		}
	//		else*/
	//		{
	//			(*itr)->SetCompleted();
	//		}
	//	}
	//	localq.erase(localq.begin(), itr);

	//	std::this_thread::sleep_for(std::chrono::microseconds(RedisCommon::IDLE_TIME));
	//}
}