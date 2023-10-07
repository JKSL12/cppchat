#include "pch.h"
#include "ClientWorker.h"

#include <iostream>
#include "GameClient.h"
#include "Client.h"
#include "ChatDlg.h"
#include "LoginDlg.h"

#define ON_RECV_PACKET(packet)			case PROTOCOL::ID::packet: \
												{	_result = OnRecv_##packet(reinterpret_cast<PROTOCOL::packet*>(_packet+1)); }	\
												break;

NET::eDISPATCH_RESULT CClientWorker::_on_dispatch_game(NET::HEADER* _packet)
{
	NET::eDISPATCH_RESULT _result = NET::eDISPATCH_RESULT::DISPATCH_FAIL;

	// 일반 유저에게 들어온 패킷이다.
	switch (_packet->type)
	{
		ON_RECV_PACKET(GS_LOGIN_RES)
		ON_RECV_PACKET(GS_CHAT_RES)
		ON_RECV_PACKET(GS_ENTERCHAT_NFY)
		ON_RECV_PACKET(GS_LEAVECHAT_NFY)

	default:
		_result = NET::eDISPATCH_RESULT::DISPATCH_OK;
	}

	return _result;
}

NET::eDISPATCH_RESULT CClientWorker::OnRecv_GS_LOGIN_RES(PROTOCOL::GS_LOGIN_RES* _res)
{
	m_parent.SetLogin(true);

	return NET::DISPATCH_OK;
}

NET::eDISPATCH_RESULT CClientWorker::OnRecv_GS_CHAT_RES(PROTOCOL::GS_CHAT_RES* _res)
{	
	m_parent.chatDlg->RecvMsg(_res->account_seq, _res->szText);

	return NET::DISPATCH_OK;
}

NET::eDISPATCH_RESULT CClientWorker::OnRecv_GS_ENTERCHAT_NFY(PROTOCOL::GS_ENTERCHAT_NFY* _res)
{	
	m_parent.chatDlg->RecvMsgEnterLeave(_res->account_seq, "입장");

	return NET::DISPATCH_OK;
}

NET::eDISPATCH_RESULT CClientWorker::OnRecv_GS_LEAVECHAT_NFY(PROTOCOL::GS_LEAVECHAT_NFY* _res)
{
	m_parent.chatDlg->RecvMsgEnterLeave(_res->account_seq, "퇴장");

	return NET::DISPATCH_OK;
}

NET::eDISPATCH_RESULT CClientWorker::OnDispatch_GAME(NET::HEADER* _packet)
{
	NET::eDISPATCH_RESULT _result = NET::eDISPATCH_RESULT::DISPATCH_INVALID_PACKET;
		
	_result = _on_dispatch_game(_packet);

	if (NET::eDISPATCH_RESULT::DISPATCH_OK != _result)
	{
		printf("[GAME_PACKET] Packet ID: 0x%08X\n", _packet->type);
	}

	return _result;
}
