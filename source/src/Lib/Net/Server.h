#pragma once

namespace NET
{
	class CNetWorker;
	class CAcceptor;

	class CServer
	{
	public:
		CServer();
		virtual ~CServer();

		bool Initialize(
			UINT16	_accept_session_max,
			UINT16	_server_session_max,
			UINT16	_io_thread_max,
			INT16	_queue_count,
			INT16	_system_dispatch_max);
		void Finalize();
		void Run(bool bRun);

		bool RegisterAcceptObj(CAcceptor* _acceptobj, INT16 _dispatch_max);

	protected:
		boost::thread_group		m_threadMgr;

	private:
		bool Start();									
		void Stop();

		virtual bool OnInitialize() { return true; }
		virtual void OnFinalize() {}
		virtual bool OnStart() { return true; }	
		virtual void OnStop() {}					
		virtual bool OnRun() { return false; }

		virtual CNetWorker* GetWorker() = 0;		

		UINT16					m_io_thread_max;
	};

}

