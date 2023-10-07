#include "pch.h"
#include "RecvQueue.h"

#include "Global.h"
#include "Packet.h"
#include "NetCore.h"

namespace NET
{
	CRecvQueue::Queue::Queue() : _dispatchmax(0), _que(1024)
	{
	}

	CRecvQueue::Queue::~Queue()
	{
	}

	void CRecvQueue::Queue::init(INT16 i_dispatchmax)
	{
		_dispatchmax = i_dispatchmax;
	}

	bool CRecvQueue::Queue::push(INT16 i_ssnid, CPacket* i_packet)
	{
		if (nullptr == i_packet)
		{
			return false;
		}

		QUEUE_INFO info;
		info._ssnid = i_ssnid;
		info._packet = i_packet;
		info._tick = GetTickCount64();

		if (false == _que.push(info))
		{
			return false;
		}
		return true;
	}

	bool CRecvQueue::Queue::pop(INT16& o_ssnid, CPacket** o_packet, ULONGLONG& o_tick)
	{
		QUEUE_INFO info;
		bool result = _que.pop(info);
		o_ssnid = info._ssnid;
		*o_packet = info._packet;
		o_tick = info._tick;

		return result;
	}

	INT16 CRecvQueue::Queue::dispatch_max()
	{
		return _dispatchmax;
	}

	CRecvQueue::CRecvQueue()
		: m_queues(NULL)
		, m_queue_count(DEFAULT_QUEUE_COUNT)
	{
	}

	CRecvQueue::~CRecvQueue()
	{
		Close();
	}

	bool  CRecvQueue::Initialize(INT16 _queue_count, INT16 _system_dispatch_max)
	{
		if (!m_queues && DEFAULT_QUEUE_COUNT <= _queue_count && _queue_count <= QUEUE_COUNT_MAX)
		{
			m_queue_count = _queue_count;

			m_queues = new Queue[m_queue_count];		// 패킷 큐 배열 생성
			// 시스템 패킷 큐 생성
			if (!Create(SYSTEM_QUEUE, _system_dispatch_max))
			{
				// 오류 발생
				return false;
			}
		}
		else
		{
			// 오류 발생
			if (m_queues)
			{
				delete m_queues;
				m_queues = nullptr;
			}
			return false;
		}
		return true;
	}

	bool CRecvQueue::Create(INT16 _queue_index, INT16 _dispatch_max)
	{
		if (_is_valid_queue(_queue_index))
		{
			Queue& _info = m_queues[_queue_index];
			_info.init(_dispatch_max);
			return true;
		}
		else
		{
			// 오류 발생
		}
		return false;
	}

	void CRecvQueue::Close()
	{
		if (m_queues)
		{
			for (INT16 i = 0; m_queue_count > i; ++i)
			{
				Queue& _info = m_queues[i];

				INT16 ssnid = INVALID_SSNID;
				CPacket* packet = nullptr;
				ULONGLONG tick = 0;
				while (_info.pop(ssnid, &packet, tick))
				{
					delete packet;
					packet = nullptr;
				}
			}

			delete[] m_queues;
			m_queues = NULL;
			m_queue_count = 0;
		}
	}

	void CRecvQueue::Push(INT16 _queue_index, INT16 _ssnid, CPacket* _packet)
	{
		if (_is_valid_queue(_queue_index) && m_queues)
		{
			Queue& _info = m_queues[_queue_index];

			/*
				packet에 대한 ref count는 큐에 push하는 순간 하나 더 증가해줘야 하지만
				그러지 않고 있다. 그래서 만약 다른 쓰레드가 바로 pop하고 패킷 핸들링끝내고
				delete packet까지 해버리면 패킷 버퍼가 해제되는데 그 이후에 이 쓰레드가
				_packet의 Type()이나 PACKET_SIZE()를 호출할 경우 크래시가 발생한다.
				그래서 ref count를 증가시켜줄게 아니라면 push전에 type, size를 미리 알아둔다
			*/
			INT32 type = _packet->Type();
			INT32 size = _packet->PACKET_SIZE();
			if (false == _info.push(_ssnid, _packet))
			{
				if (GLOBAL::NetCore.IsRun())
				{
					printf("push packet error - queue_index: % d, ssnid : % d\n", _queue_index, _ssnid);
				}
			}
			else
			{
				//if (SYSTEM_QUEUE != _queue_index)
				{
					
				}
			}
		}
	}

	bool CRecvQueue::Pop(INT16 _queue_index, INT16& _ssnid, CPacket** _packet, ULONGLONG& tick)
	{
		if (_is_valid_queue(_queue_index))
		{
			Queue& _info = m_queues[_queue_index];
			if (false == _info.pop(_ssnid, _packet, tick))
			{
				return false;
			}
		}
		return true;
	}

	INT16 CRecvQueue::GetDispatchMax(INT16 _queue_index)
	{
		return _is_valid_queue(_queue_index) ? m_queues[_queue_index].dispatch_max() : 0;
	}

	INT16 CRecvQueue::GetQueueCount()
	{
		return m_queue_count;
	}
}