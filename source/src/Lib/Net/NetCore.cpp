#include "pch.h"
#include "NetCore.h"

#include "GameServer/PROTOCOL/Protocol_SYSTEM.h"
#include "Global.h"
#include "Session.h"
#include "RecvQueue.h"
#include "Packet.h"
#include "SessionManager.h"

#define NEW_SYSTEM_PACKET(type)					CPacket * pPacket = new CPacket( PROTOCOL::ID::type, sizeof( PROTOCOL::type ) ); \
												PROTOCOL::type * pCommand = static_cast< PROTOCOL::type * >( pPacket->Body() );

namespace NET
{
	CNetCore::CNetCore()
		: m_bRun(false)
	{
	}

	CNetCore::~CNetCore()
	{
	}

	// 멤버 함수 구현
	void CNetCore::Start()
	{
		m_bRun = true;
	}

	void CNetCore::Stop()
	{
		m_bRun = false;
	}

	// 네트워크 구동 함수
	void CNetCore::Run()
	{
		GLOBAL::Io_Context.run();
	}

	void CNetCore::Dispatch(CPacket* pPacket)
	{
		switch (pPacket->Type())
		{
		case PROTOCOL::ID::SM_CLOSE_SESSION_REQ:	// 세션 닫기
			Recv_CLOSE_SESSION_REQ(pPacket->Body());
			break;
		default:
			break;
		}
	}

	void CNetCore::Recv_CLOSE_SESSION_REQ(void* pData)
	{
		PROTOCOL::SM_CLOSE_SESSION_REQ* pPacket = static_cast<PROTOCOL::SM_CLOSE_SESSION_REQ*>(pData);

		std::shared_ptr<CSession> pSession = GLOBAL::SessionMgr.Find(pPacket->SSNID);
		if (nullptr != pSession)
		{
			pSession->_close_handler();
			pSession->_close();
		}
	}
	
	void CNetCore::Send_OPEN_SESSION_REQ(std::shared_ptr<CSession> pSession)
	{
		boost::asio::ip::tcp::endpoint _remote = pSession->RemotePoint();
		boost::asio::ip::address  _addr = _remote.address();

		size_t cn;

		WCHAR _ip_addr[64] = { 0, };

		mbstowcs_s(&cn, _ip_addr, _addr.to_string().c_str(), _countof(_ip_addr));

		//----------------------------------------------------------------------

		NEW_SYSTEM_PACKET(SM_OPEN_SESSION_REQ);

		pCommand->Type = pSession->Type();
		pCommand->SSNID = pSession->SSNID();

		_tcscpy_s(pCommand->IP, 20, _ip_addr);
		pCommand->Port = _remote.port();

		_system_send(pPacket);

		//InterlockedIncrement( &GLOBAL::NET_STATS.connections );
	}

	// 세션 끊기 (닫기)
	void CNetCore::Send_CLOSE_SESSION_REQ(std::shared_ptr<CSession> pSession)
	{
		if (true == pSession->IsClose())
			return;

		NEW_SYSTEM_PACKET(SM_CLOSE_SESSION_REQ);

		pCommand->SSNID = pSession->SSNID();
		pCommand->Mode = 0;

		_system_send(pPacket);
	}

	void CNetCore::Send_FAIL_CONNECT_REQ(INT8 type)
	{
		NEW_SYSTEM_PACKET(SM_FAIL_CONNECT_REQ);

		pCommand->Type = type;

		_system_send(pPacket);
	}

	void CNetCore::_system_send(CPacket* pPacket)
	{
		GLOBAL::RecvQue.Push(SYSTEM_QUEUE, INVALID_SSNID, pPacket);
	}
}