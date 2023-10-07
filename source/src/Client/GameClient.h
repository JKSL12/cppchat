#pragma once

#include "Net/Session.h"
#include "ClientWorker.h"

class CChatDlg;

class CGameClient
{
public:
	CGameClient(CChatDlg* dlg, INT64 account_seq);
	CGameClient();
	~CGameClient();

	bool Initialize(const WCHAR* szAddress, UINT16 nPort);

	void Connect();
	void Run();	

	void SetLogin(bool login) { m_Login = login; }

	void UpdateRoutines();

	NET::CSession* GetSession() { return _session.get(); }

public:
	CChatDlg* chatDlg;

private:
	std::shared_ptr<NET::CSession> _session;

	boost::thread_group m_threadMgr;

	CClientWorker m_worker;

	INT64 updateTime;
	INT64 m_account_seq;

	bool m_Login;
	bool m_bRun;
};

