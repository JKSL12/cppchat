#pragma once

#include "UserAcceptor.h"
#include "Net/Server.h"

class CGameServer : public NET::CServer
{
public:
	CGameServer();
	virtual ~CGameServer();

	eSERVER_RESULT PreInitialize();

private:
	virtual bool OnInitialize();
	virtual void OnFinalize();
	virtual bool OnStart();
	virtual void OnStop();
	virtual bool OnRun();

	virtual CGameWorker* GetWorker();

private:
	bool				m_bRun;
	CGameWorker			m_Worker;
	SERVER::eSERVER_STATE m_state;

	CUserAcceptor		m_UserAcceptor;

	friend class CGameWorker;
};

