#include "pch.h"
#include "GameWorker.h"

#include "../SERVER/GameServer.h"
#include "Net/Packet.h"
#include "Net/Session.h"
#include "Net/SessionManager.h"
#include "OBJECT/UserManager.h"
#include "OBJECT/User.h"

CGameWorker::CGameWorker(CGameServer& parent)
	: m_parent(parent)
{
	_userObjectAlloc.Reserve(100);
}

CGameWorker::~CGameWorker()
{
}

bool CGameWorker::OnFinalize(Object* user)
{
	CUser* userObj = (CUser*)user;

	GLOBAL::USER_MANAGER.Remove(userObj);

	return true;
}

NET::eDISPATCH_RESULT CGameWorker::OnDispatch(INT16 _ssnid, NET::CPacket* _packet, ULONGLONG arrivedTick)
{
	/*if (PROTOCOL::ID::SESSION_TUNNELING == _packet->Type())
	{
		INT16* _count = static_cast<INT16*>(_packet->Body());
		BYTE* _pos = reinterpret_cast<BYTE*>(_count + 1);

		for (INT16 i = 0; *_count > i; ++i)
		{
			NET::HEADER* _header = reinterpret_cast<NET::HEADER*>(_pos);

			_on_dispatch(_ssnid, _header, arrivedTick);

			auto size = _header->size + NET::HEADER_SIZE;

			_pos += size;
		}
	}
	else*/
	{
		return _on_dispatch(_ssnid, _packet->Header(), arrivedTick);
	}

	return NET::eDISPATCH_RESULT::DISPATCH_OK;
}

NET::eDISPATCH_RESULT CGameWorker::_on_dispatch(INT16 _ssnid, NET::HEADER* _packet, ULONGLONG arrivedTick)
{
	NET::eDISPATCH_RESULT _result = NET::eDISPATCH_RESULT::DISPATCH_FAIL;

	switch (PROTOCOL::GET_PROTOCOL_ID(_packet->type))
	{
	case PROTOCOL::ID::PROTOCOL_GAME:		_result = OnDispatch_GAME(_ssnid, _packet);		break;

	default:
		_result = NET::eDISPATCH_RESULT::DISPATCH_INVALID_PACKET;
		printf("[PACKET] SSNID: %d%, Packet ID: 0x%08X [%d]\n",
			_ssnid, _packet->type, _result);
	}

	return _result;
}

NET::eDISPATCH_RESULT CGameWorker::OnOpenSession(INT8 _type, INT16 _ssnid, const TCHAR* _ip_addr, UINT16 _port)
{
	TCHAR* szType = const_cast<TCHAR*>(TEXT(""));

	switch (_type)
	{
	case NET::USER_SESSION:
	{
		// SSNID �� SsnMngr ���� �߱��� ������ �־�� �Ѵ�.
		std::shared_ptr<NET::CSession> _session = NET::GLOBAL::SessionMgr.Find(_ssnid);
		//auto	_session = TLNET::SessionManager::Instance()->Seek(_ssnid);

		if (!_session)
		{
			// ���� �����忡�� ���� ���� ó���� �ϱ� ���� ������ ����� �������� ������ �ȴ�.
			// �αװ� �ʹ� ���� ���Ƽ� �ּ� ó��. Global Accelerator �� ���μ����� ����ִ��� ��� üũ�ϴ� �� ����.
			//_ERROR_LOG("CGameWorker::OnOpenSession(_type = %d, _ssnid = %d, _ip_addr, _port = %d)", _type, _ssnid, _port);
			return NET::eDISPATCH_RESULT::DISPATCH_INVALID_SESSION;
		}
				
		bool failed = false;
		bool serverblock = false;

		/*if (!GLOBAL::FIELD_MANAGER.IsDBLoadComplete())
			failed = true;*/

		

		//if (failed)	// DB �ε尡 ������ �ʾҰų� ��ϵ� �ְų� white IP�� ������ �ִ� ���.
		//{
		//	_ERROR_LOG("CGameWorker::OnOpenSession(_type = %d, _ssnid = %d, _ip_addr, _port = %d) serverblock = %d", _type, _ssnid, _port, serverblock);

		//	INT32 result = RESULT::R_LOGIN_FAIL_SERVERSTARTING;

		//	if (serverblock)
		//		result = RESULT::R_LOGIN_FAIL_SERVERBLOCK;

		//	NEW_FLATBUFFER_NOLAMBDA(GS_LOGOUT_NFY, pPACKET);
		//	pPACKET.Build(PROTOCOL::FLATBUFFERS::CreateGS_LOGOUT_NFY(fbb, result, -1));
		//	END_FLATBUFFER_NOLAMBDA;
		//	Send_SSNID(_ssnid, pPACKET.PACKET());
		//	return NET::eDISPATCH_RESULT::DISPATCH_INVALID_SESSION;
		//}			

		//GLOBAL::KEEP_ALIVE_MANAGER.Insert(_ssnid);
	}
	break;

	default:
	{
		printf("�� �� ���� ���� Ÿ�� �Դϴ�(Type: %d, SSNID: %d)\n", _type, _ssnid);
		return NET::eDISPATCH_RESULT::DISPATCH_INVALID_SESSION;
	}
	}
	
	printf("Open %s Session (Type: %d, SSNID: %d, IP: %s, Port: %d)\n",
		szType, _type, _ssnid, _ip_addr, _port);

	return NET::eDISPATCH_RESULT::DISPATCH_OK;
}

NET::eDISPATCH_RESULT CGameWorker::OnCloseSession(INT16 _ssnid, INT8 _mode)
{
	TCHAR* _type = const_cast<TCHAR*>(TEXT(""));
	CUser* user = GLOBAL::USER_MANAGER.FindActiveUserFromSSNID(_ssnid);

	if (user)
	{
		//GLOBAL::KEEP_ALIVE_MANAGER.Remove(_ssnid);
		GLOBAL::USER_MANAGER.ForAllUsers([=](CUser* pUser) {
			if (pUser->account_seq == user->account_seq) return;

			auto packet = new NET::CPacket(PROTOCOL::ID::GS_LEAVECHAT_NFY, sizeof(PROTOCOL::GS_LEAVECHAT_NFY));
			static_cast<PROTOCOL::GS_LEAVECHAT_NFY*>(packet->Body())->account_seq = user->account_seq;

			CNetWorker::Send(pUser->SSNID(), packet);
			});

		_type = const_cast<TCHAR*>(TEXT("User"));

		user->Release();
	}
	//else if (GLOBAL::SESSION_MANAGER.Find(_ssnid))
	//
		//GLOBAL::KEEP_ALIVE_MANAGER.Remove(_ssnid);
	//}
	else
	{		
		{
			// ���� �����忡�� ���� ���� ó���� �ϱ� ���� ������ ����� �������� ������ �ȴ�.
			// CUserManager::Inactivate()�� ȣ���� �� logout �� true �� �Է��� �� ���� ��ü�� �ٷ� ����������� ��쿡�� �������� ������ �ȴ�.
			// �αװ� �ʹ� ���� ���Ƽ� �ּ� ó��. Global Accelerator �� ���μ����� ����ִ��� ��� üũ�ϴ� �� ����.
			//_ERROR_LOG("�������� �ʴ� ���� �Դϴ�(SSNID: %d)", _ssnid);
			return NET::eDISPATCH_RESULT::DISPATCH_INVALID_SESSION;
		}
	}

	printf("Close %s Session (SSNID: %d, Mode: %d)\n", _type, _ssnid, _mode);
	return NET::eDISPATCH_RESULT::DISPATCH_OK;
}

NET::eDISPATCH_RESULT CGameWorker::OnFailConnect(INT8 _type)
{
	printf("Fail Connect (Type: %d)\n", _type);

	//NET::GLOBAL::CnntObjMngr.Connect(_type);

	return NET::DISPATCH_OK;
}

void CGameWorker::Send_SSNID(INT16 _ssnid, NET::CPacket* _packet)
{
	if (NET::INVALID_SSNID == _ssnid)
		return;

	/*if (_packet->Header()->checksum == 10)
	{
		_ERROR_LOG("CGameWorker::Send_SSNID(_ssnid = %d, _packet) id:[%d], size:[%d], packet name : %s", _ssnid, _packet->Header()->type, _packet->Size(), TLNET::CPacketInfo::PacketIdToName(_packet->Header()->type).c_str());
		GLOBAL::SendLogErrorPacket(_ssnid, _packet->Header()->type, _packet->Size());
		return;
	}*/

	CNetWorker::Send(_ssnid, _packet);
}

NET::eDISPATCH_RESULT CGameWorker::Send_USER(const CUser* pUser, NET::CPacket* pPacket)
{
	if (pUser)
	{
		/*if (pPacket->Header()->checksum == 10)
		{
			_ERROR_LOG("CGameWorker::Send_USER(pUser, pPacket) pUser->SSNID() = %d, id:[%d], size:[%d], packet name : %s", pUser->SSNID(), pPacket->Header()->type, pPacket->Size(), TLNET::CPacketInfo::PacketIdToName(pPacket->Header()->type).c_str());
			GLOBAL::SendLogErrorPacket(pUser->SSNID(), pPacket->Header()->type, pPacket->Size());
			return NET::DISPATCH_OK;
		}*/

		CNetWorker::Send(pUser->SSNID(), pPacket);
	}

	return NET::DISPATCH_OK;
}

void CGameWorker::Send_ALL(NET::CPacket* pPacket)
{
	
}