#include "pch.h"
#include "GameClient.h"

#include <iostream>
#include "Net/Global.h"
#include "Net/NetCore.h"
#include "Net/NetWorker.h"
#include "Net/RecvQueue.h"
#include "Net/Packet.h"
#include "ChatDlg.h"

CGameClient::CGameClient(CChatDlg* dlg, INT64 account_seq)
	: chatDlg(dlg)
	, m_account_seq(account_seq)
	, m_Login(false)
	, m_worker(*this)
	, m_bRun(false)
{
	_session = std::shared_ptr<NET::CSession>(new NET::CSession(static_cast<INT8>(NET::USER_SESSION), NET::USER_QUEUE, false));
}

CGameClient::CGameClient()
	: m_Login(false)
	, m_worker(*this)
	, m_bRun(false)
{
	_session = std::shared_ptr<NET::CSession>(new NET::CSession(static_cast<INT8>(NET::USER_SESSION), NET::USER_QUEUE, false));
}

CGameClient::~CGameClient()
{

}

bool CGameClient::Initialize(const WCHAR* szAddress, UINT16 nPort)
{
	NET::GLOBAL::NetCore.Start();

	if (!NET::GLOBAL::RecvQue.Initialize(2, 1))
	{
		return false;
	}

	//m_threadMgr.create_thread(boost::bind(&NET::CNetWorker::Run_ALL, NET::CNetWorker));

	boost::system::error_code	error;

	size_t wcn;
	char szTemp[16] = { 0, };
	wcstombs_s(&wcn, szTemp, szAddress, _countof(szTemp));
	
	boost::asio::ip::tcp::endpoint	endpoint(boost::asio::ip::address::from_string(szTemp, error), nPort);

	if (error)
	{
		printf("%s\n", error.message().c_str());
		return false;
	}

	_session->Socket().connect(endpoint, error);

	if (error)
	{
		boost::asio::ip::address _addr = endpoint.address();

		size_t mbn;

		WCHAR _ip_addr[64] = { 0, };

		mbstowcs_s(&mbn, _ip_addr, _addr.to_string().c_str(), _countof(_ip_addr));

		printf("Connect failed (IP: %s, Port: %d)\n", _ip_addr, endpoint.port());

		return false;
	}

	m_threadMgr.create_thread(boost::bind(&CClientWorker::Run, m_worker));

	m_bRun = true;

	updateTime = GetTickCount64();

	return true;
}

void CGameClient::Connect()
{
	NET::GLOBAL::RecvQue.Create(NET::USER_QUEUE, 1);

	//NET::GLOBAL::NetCore.Send_OPEN_SESSION_REQ(_session);

	_session->_recv();

	auto packet = new NET::CPacket(PROTOCOL::ID::GS_LOGIN_REQ, sizeof(PROTOCOL::GS_LOGIN_REQ));
	static_cast<PROTOCOL::GS_LOGIN_REQ*>(packet->Body())->account_seq = m_account_seq;
	_session->_send(packet);
}

void CGameClient::Run()
{
	m_threadMgr.create_thread(boost::bind(&NET::CNetCore::Run, &NET::GLOBAL::NetCore));

	//while (m_bRun)
	//{
	//	;
	//	//char cz[512];
	//	////wscanf_s(L"%s", sz, _countof(sz));

	//	//std::cin.getline(cz, _countof(cz), '\n');

	//	////_tcscanf_s(_T("%s"), sz, _countof(sz));
	//	//auto packet = new NET::CPacket(PROTOCOL::ID::GS_TEST_REQ, sizeof(PROTOCOL::GS_TEST_REQ));
	//	////_tcsncpy_s(static_cast<PROTOCOL::GS_TEST_REQ*>(packet->Body())->szText, cz, _countof(cz));
	//	//strncpy_s(static_cast<PROTOCOL::GS_TEST_REQ*>(packet->Body())->szText, cz, _countof(cz));
	//	//_session->_send(packet);
	//}
}

void CGameClient::UpdateRoutines()
{
	if (m_Login == false) return;

	if (GetTickCount64() < updateTime + 1000) return;
	
	updateTime = GetTickCount64();
		
	
}

