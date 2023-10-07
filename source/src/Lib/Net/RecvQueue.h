#pragma once

namespace NET
{
	class CPacket;

	class CRecvQueue
	{
		struct QUEUE_INFO
		{
			INT16 _ssnid = 0;
			CPacket* _packet = nullptr;
			ULONGLONG _tick;
		};

		class Queue
		{
		private:
			typedef boost::lockfree::queue<
				QUEUE_INFO
			> LOCKFREE_QUEE;

		public:
			Queue();
			~Queue();

			void init(INT16 i_dispatchmax);
			bool push(INT16 i_ssnid, CPacket* i_packet);
			bool pop(INT16& o_ssnid, CPacket** o_packet, ULONGLONG& o_tick);

			INT16 dispatch_max();

		private:
			INT16 _dispatchmax;
			LOCKFREE_QUEE _que;
		};

	public:
		CRecvQueue();
		~CRecvQueue();

		bool	Initialize(INT16 _queue_count, INT16 _system_dispatch_max);

		bool	Create(INT16 _queue_index, INT16 _dispatch_max);
		void	Close();

		void	Push(INT16 _queue_index, INT16 _ssnid, CPacket* _packet);
		bool	Pop(INT16 _queue_index, INT16& _ssnid, CPacket** _packet, ULONGLONG& tick);

		INT16	GetDispatchMax(INT16 _queue_index);
		INT16	GetQueueCount();

	private:
		inline bool _is_valid_queue(INT16 _queue_index)
		{
			return 0 <= _queue_index && _queue_index <= m_queue_count;
		}

		Queue*			m_queues;			// 패킷 큐 배열
		INT16			m_queue_count;		// 패킷 큐 개수
	};
}
