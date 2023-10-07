#include "pch.h"
#include "GameWorker.h"
#include "NET/SessionManager.h"
#include "NET/Session.h"
#include "OBJECT/User.h"
#include "OBJECT/UserManager.h"

#pragma warning (disable : 4239)

#define ON_RECV_CONNECT_PACKET(packet)	case PROTOCOL::ID::packet: \
												{	_result = OnRecv_##packet(_ssnid,reinterpret_cast<PROTOCOL::packet*>(_packet+1));	}	\
												break;


#define ON_RECV_PACKET(var, packet)			case PROTOCOL::ID::packet: \
												{	_result = OnRecv_##packet(var,reinterpret_cast<PROTOCOL::packet*>(_packet+1)); }	\
												break;



NET::eDISPATCH_RESULT CGameWorker::_on_dispatch_game(CUser * _user, NET::HEADER * _packet)
{
	NET::eDISPATCH_RESULT _result = NET::eDISPATCH_RESULT::DISPATCH_FAIL;

	// 일반 유저에게 들어온 패킷이다.
	switch (_packet->type)
	{
		ON_RECV_PACKET(_user, GS_CHAT_REQ)

	default:
		_result = NET::eDISPATCH_RESULT::DISPATCH_OK;
	}

	return _result;
}

NET::eDISPATCH_RESULT CGameWorker::OnDispatch_GAME(INT16 _ssnid, NET::HEADER * _packet)
{	
	std::shared_ptr<NET::CSession> _session = NET::GLOBAL::SessionMgr.Find(_ssnid);
	//auto	_session = TLNET::SessionManager::Instance()->Seek(_ssnid);

	//if (_session)
	//{
	//	bool isHack = _session->IsSpeedHack();
	//	if (isHack)
	//	{
	//		CUser * _user = GLOBAL::USER_MANAGER.FindActiveUserFromSSNID(_ssnid);
	//		if (_user)
	//			_INFO_LOG_FILE("speed_hack.txt", "UID:%I64d", _user->UID());

	//		// 스피드핵 감지 로그
	//		if (_user && _packet)
	//		{
	//			Send_GS_LOGOUT_NFY(_user, RESULT::R_FAIL_ABNORMAL_CLIENT, _packet->type);
	//			NEW_DB_LOG_SYSTEM(log, DB_LOG::COMMAND_SPEED_HACK);
	//			log.set_uid(_user->UID());
	//			log.set_reason(DB_LOG::REASON_SPEED_HACK_CLOSE_SESSION);

	//			log.p(0, _packet->type);
	//			IF_VALID_DB_LOG(log)
	//			{
	//				WRITE_DB_LOG(log);
	//			}
	//		}

	//		CloseSession(_ssnid);
	//	}
	//}
		
	NET::eDISPATCH_RESULT _result = NET::eDISPATCH_RESULT::DISPATCH_INVALID_PACKET;

	if (PROTOCOL::ID::eGAME_PACKET_ID::GS_PACKET_BEGIN < _packet->type &&
		PROTOCOL::ID::eGAME_PACKET_ID::GS_CONNECTION_PACKET_END > _packet->type)
	{
		switch (_packet->type)
		{
			ON_RECV_CONNECT_PACKET(GS_LOGIN_REQ)			// ON_RECV_CONNECT_FLATBUFFERS(_packet->flat, GS_LOGIN_REQ)
			ON_RECV_CONNECT_PACKET(GS_KEEP_ALIVE_NFY)
		}
	}
	else
	{
		CUser * _user = GLOBAL::USER_MANAGER.FindActiveUserFromSSNID(_ssnid);

		_result = _user ? _on_dispatch_game(_user, _packet)
			: NET::eDISPATCH_RESULT::DISPATCH_INVALID_SESSION;
	}

	if (NET::eDISPATCH_RESULT::DISPATCH_OK != _result)
	{
		printf("[GAME_PACKET] SSNID: %d, Packet ID: 0x%08X\n",
			_ssnid, _packet->type);
	}

	return _result;
}
