#include "pch.h"
#include "Server.h"

#include "Global.h"
#include "RecvQueue.h"
#include "NetCore.h"
#include "SessionManager.h"
#include "NetWorker.h"
#include "Acceptor.h"

namespace NET
{
	CServer::CServer() : m_io_thread_max(1)
	{
	}

	CServer::~CServer()
	{
	}

	bool CServer::Initialize(
		UINT16	_accept_session_max,		// Client(User) ���� ����
		UINT16	_server_session_max,		// Server(Connect) ���� ����
		UINT16	_io_thread_max,				// I/O ������ ����
		INT16	_queue_count,				// Recv Queue ����
		INT16	_system_dispatch_max)		// 1�����Ӵ� �ý��� �޽��� ����
	{
		m_io_thread_max = _io_thread_max;

		GLOBAL::NetCore.Start();

		if (!GLOBAL::RecvQue.Initialize(_queue_count, _system_dispatch_max))
		{
			return false;
		}

		GLOBAL::SessionMgr.Initialize(_accept_session_max, _server_session_max);

		m_threadMgr.create_thread(boost::bind(&NET::CNetWorker::Run_ALL, GetWorker()));

		return OnInitialize();
	}

	void CServer::Finalize()
	{
		m_threadMgr.join_all();		// ������ ���� ���

		OnFinalize();
	}

	bool CServer::RegisterAcceptObj(CAcceptor* _acceptobj, INT16 _dispatch_max)
	{
		return GLOBAL::RecvQue.Create(_acceptobj->QueueIndex(), _dispatch_max);
	}

	bool CServer::Start()
	{
		bool result = OnStart();
		// STEP 2: I/O Thread ���� (����)
		for (int i = 0; m_io_thread_max > i; ++i)
		{
			m_threadMgr.create_thread(boost::bind(&NET::CNetCore::Run, &GLOBAL::NetCore));
		}
		return result;
	}

	void CServer::Stop()
	{
		OnStop();

		GLOBAL::NetCore.Stop();

		GLOBAL::SessionMgr.Finalize();

		while (GLOBAL::SessionMgr.ActiveCount())
		{
			// ������ ��� ������ ������ ���
			Sleep(100);
		}

		GLOBAL::RecvQue.Close();
	}

	void CServer::Run(bool bRun)
	{
		if (bRun && Start())
		{
			ULONGLONG _last = ::GetTickCount64();

			while (OnRun())
			{
				ULONGLONG _now = ::GetTickCount64();
				FLOAT _delta = 1000.0f / (_now - _last);

				_last = _now;
			}
		}

		Stop();
	}
}