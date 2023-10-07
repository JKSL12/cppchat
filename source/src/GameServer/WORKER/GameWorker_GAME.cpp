
// GameWorker_GAME.cpp : CGameWorker 클래스 구현
//

#include "pch.h"
#include "GameWorker.h"

#include <iostream>
#include "Net/Packet.h"
#include "OBJECT/UserManager.h"
#include "OBJECT/User.h"

NET::eDISPATCH_RESULT CGameWorker::OnRecv_GS_LOGIN_REQ(INT16 _ssnid, PROTOCOL::GS_LOGIN_REQ* _req)
{
	auto _user = new CUser(_ssnid);
	_user->account_seq = _req->account_seq;
	GLOBAL::USER_MANAGER.Insert(_user);

	auto packet = new NET::CPacket(PROTOCOL::ID::GS_LOGIN_RES, sizeof(PROTOCOL::GS_LOGIN_RES));
	Send_SSNID(_ssnid, packet);

	GLOBAL::USER_MANAGER.ForAllUsers([=](CUser* pUser) {
		if (pUser->account_seq == _req->account_seq) return;

		auto packet = new NET::CPacket(PROTOCOL::ID::GS_ENTERCHAT_NFY, sizeof(PROTOCOL::GS_ENTERCHAT_NFY));
		static_cast<PROTOCOL::GS_ENTERCHAT_NFY*>(packet->Body())->account_seq = _req->account_seq;

		CNetWorker::Send(pUser->SSNID(), packet);
		});

	return NET::DISPATCH_OK;
}

NET::eDISPATCH_RESULT CGameWorker::OnRecv_GS_KEEP_ALIVE_NFY(INT16 _ssnid, PROTOCOL::GS_KEEP_ALIVE_NFY* _req)
{

	return NET::DISPATCH_OK;
}

NET::eDISPATCH_RESULT CGameWorker::OnRecv_GS_CHAT_REQ(CUser* _user, PROTOCOL::GS_CHAT_REQ* _req)
{
	std::cout << "OnRecv_GS_CHAT_REQ " << _req->szText << "\n";
	//_tcprintf_s(_T("OnRecv_GS_TEST_REQ %s\n"), _req->szText);

	GLOBAL::USER_MANAGER.ForAllUsers([=](CUser* pUser) {
		auto packet = new NET::CPacket(PROTOCOL::ID::GS_CHAT_RES, sizeof(PROTOCOL::GS_CHAT_RES));
		//static_cast<PROTOCOL::GS_CHAT_RES*>(packet->Body())->account_seq = _user->SSNID();
		static_cast<PROTOCOL::GS_CHAT_RES*>(packet->Body())->account_seq = _req->account_seq;
		//_tcsncpy_s(static_cast<PROTOCOL::GS_TEST_RES*>(packet->Body())->szText, _req->szText, sizeof(_req->szText));		
		strncpy_s(static_cast<PROTOCOL::GS_CHAT_RES*>(packet->Body())->szText, _req->szText, sizeof(_req->szText));

		CNetWorker::Send(pUser->SSNID(), packet);
		});

	/*auto packet = new NET::CPacket(PROTOCOL::ID::GS_TEST_RES, sizeof(PROTOCOL::GS_TEST_RES));
	static_cast<PROTOCOL::GS_TEST_RES*>(packet->Body())->ssnid = _user->SSNID();
	_tcsncpy_s(static_cast<PROTOCOL::GS_TEST_RES*>(packet->Body())->szText, _req->szText, sizeof(_req->szText));
	Send_USER(_user, packet);*/

	return NET::DISPATCH_OK;
}
