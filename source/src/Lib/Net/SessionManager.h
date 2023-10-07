#pragma once

namespace NET
{
	class CSession;

	class CSessionManager
	{
		class _QUEUE_SSNID
		{
			class _SSNID
			{
			public:
				typedef std::set< INT16 >		SET;
			};

		public:
			_QUEUE_SSNID();
			~_QUEUE_SSNID();

			void	reserve(const INT16 _capacity, const INT16 _base = 0);

			bool	push(const INT16 _ssnid);
			INT16	pop();

			INT16	capacity() { return m_capacity; }		// 최대 크기
			INT16	size() { return static_cast<INT16>(m_set_ssnids.size()); }	// 실제 데이터 개수

		private:
			_SSNID::SET		m_set_ssnids;	
			INT16			m_base;			
			INT16			m_capacity;		
		};

		class _ARRAY_SESSION
		{
			// 배열(Array) 데이터 구조를 클래스화 시킴
			// - 인덱스(SSNID)를 이용하여 빠르게 액세스하는 것이 목적

			class _SESSION
			{
			private:
				typedef boost::pool_allocator< std::shared_ptr<CSession> >	_Alloc;

			public:
				typedef	std::vector< std::shared_ptr<CSession>/*, _Alloc*/ >	VECTOR;
			};

		public:
			_ARRAY_SESSION();
			~_ARRAY_SESSION();

			void	reserve(const INT16 _capacity);

			void	insert(const INT16 _index, std::shared_ptr<CSession> _session);
			void	remove(const INT16 _index);
			std::shared_ptr<CSession>	get(const INT16 _index);
			INT16	findemptyssnid();

			INT16	capacity() { return static_cast<INT16>(m_sessions.size()); }
			INT16	active() { return m_active; }

		private:
			_SESSION::VECTOR	m_sessions;
			INT16				m_active;	
		};

	public:
		bool		Initialize(INT16 _capacity = SESSION_MAX,
			INT16 _server_capacity = SERVER_SESSION_MAX);
		void		Finalize();

		INT16		Insert(std::shared_ptr<CSession> _session);
		bool		Remove(INT8 uType, INT16 _ssnid);
		std::shared_ptr<CSession>		Find(INT16 _ssnid);

		INT16		ActiveCount() { return m_active_sessions.active(); }

		std::shared_ptr<CSession>	make_ptr(INT8  type = USER_SESSION, INT16 _queue_index = USER_QUEUE, bool  bEncrypt = false);

	private:
		void			destroy(CSession* i_obj);

		boost::shared_mutex m_lock;

		_ARRAY_SESSION	m_active_sessions;
		_QUEUE_SSNID	m_inactive_ssnids;
	};

}