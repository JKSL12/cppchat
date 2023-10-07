#include "pch.h"
#include "NetWorker.h"

#include "Global.h"
#include "RecvQueue.h"
#include "SendQueue.h"
#include "NetCore.h"
#include "Packet.h"

#define ON_RECV_SYSTEM_PACKET(packet)		case PROTOCOL::ID::packet: \
												result = On##packet( SSNID, static_cast< PROTOCOL::packet * >( pPacket->Body() ) ); \
												break;

namespace NET
{
	CNetWorker::CNetWorker()
	{
	}

	CNetWorker::~CNetWorker()
	{
	}

	void CNetWorker::Run_ALL()
	{
		if (OnInitialize())
		{
			INT16 _queue_count = GLOBAL::RecvQue.GetQueueCount();

			while (GLOBAL::NetCore.IsRun())
			{
				bool needSleep = _run_dispatch_system();					// 시스템 패킷 큐

				for (INT16 _queue_index = USER_QUEUE; _queue_count > _queue_index; ++_queue_index)
				{
					needSleep &= _run_on_dispatch(_queue_index);	// 일반 패킷 큐
				}

				UpdateRoutines();						// 업데이트 루틴
								
				if (needSleep)
				{
					::Sleep(1);
				}
			}

			// 종료 처리
			OnFinalize();
		}
	}

	void CNetWorker::Send(INT16 _ssnid, CPacket* _packet)
	{
		if (_ssnid == -1)
			return;

		//printf("Send Packet %d, %d\n", _ssnid, _packet->Type());

		_packet->AddRef();
		GLOBAL::SendQue.Push(_ssnid, _packet);
	}

	bool CNetWorker::_run_dispatch_system()
	{
		INT16	  _ssnid = INVALID_SSNID;
		CPacket* _packet = NULL;
		ULONGLONG _tick = 0;
		bool needSleep = true;

		INT16 _dispatch_max = GLOBAL::RecvQue.GetDispatchMax(SYSTEM_QUEUE);

		for (int i = 0; (_dispatch_max > i) && (GLOBAL::NetCore.IsRun()); ++i)
		{
			if (GLOBAL::RecvQue.Pop(SYSTEM_QUEUE, _ssnid, &_packet, _tick))
			{
				Dispatch_SYSTEM(_ssnid, _packet);
				needSleep = false;

				delete _packet;
			}
			else
			{
				needSleep = true;
				break;
			}
		}

		return needSleep;
	}

	// 일반 메시지를 꺼내는 함수 (일반 메시지 Dispatch, 지정된 패킷 큐(채널)만 처리)
	bool CNetWorker::_run_on_dispatch(INT16 _queue_index)
	{
		INT16	  _ssnid = INVALID_SSNID;
		CPacket* _packet = NULL;
		ULONGLONG _tick = 0;
		bool needSleep = true;

		INT16 _dispatch_max = GLOBAL::RecvQue.GetDispatchMax(_queue_index);

		for (int i = 0; (_dispatch_max > i) && (GLOBAL::NetCore.IsRun()); ++i)
		{
			if (GLOBAL::RecvQue.Pop(_queue_index, _ssnid, &_packet, _tick))
			{
				if (DISPATCH_OK != OnDispatch(_ssnid, _packet, _tick))
				{
					CloseSession(_ssnid);
				}
				else
				{
					needSleep = false;
				}

				delete _packet;
			}
			else
			{
				needSleep = true;
				break;
			}
		}

		return needSleep;
	}

	void CNetWorker::Dispatch_SYSTEM(INT16 SSNID, CPacket* pPacket)
	{
		NET::eDISPATCH_RESULT result = NET::eDISPATCH_RESULT::DISPATCH_FAIL;

		if (INVALID_SSNID == SSNID)
		{
			switch (pPacket->Type())
			{
				ON_RECV_SYSTEM_PACKET(SM_OPEN_SESSION_REQ)	// 세션 열림(생성)
				ON_RECV_SYSTEM_PACKET(SM_CLOSE_SESSION_REQ)	// 세션 닫음
				ON_RECV_SYSTEM_PACKET(SM_FAIL_CONNECT_REQ)	// Connect 실패(오류)

			default:
				//result = OnDispatch_SYSTEM(SSNID, pPacket);
				break;
			}
		}
	}

	eDISPATCH_RESULT CNetWorker::OnSM_OPEN_SESSION_REQ(INT16 SSNID, PROTOCOL::SM_OPEN_SESSION_REQ* pPacket)
	{
		(void*)SSNID;

		return OnOpenSession(pPacket->Type, pPacket->SSNID, pPacket->IP, pPacket->Port);
	}

	// 시스템 메시지 핸들러 (세션 닫음)
	eDISPATCH_RESULT CNetWorker::OnSM_CLOSE_SESSION_REQ(INT16 SSNID, PROTOCOL::SM_CLOSE_SESSION_REQ* pPacket)
	{
		(void*)SSNID;

		return OnCloseSession(pPacket->SSNID, pPacket->Mode);
	}

	// 시스템 메시지 핸들러 (Connect 실패(오류))
	eDISPATCH_RESULT CNetWorker::OnSM_FAIL_CONNECT_REQ(INT16 SSNID, PROTOCOL::SM_FAIL_CONNECT_REQ* pPacket)
	{
		(void*)SSNID;

		return OnFailConnect(pPacket->Type);
	}

	void CNetWorker::CloseSession(INT16 SSNID)
	{
		CPacket* pPacket = new CPacket(PROTOCOL::ID::SM_CLOSE_SESSION_REQ, sizeof(PROTOCOL::SM_CLOSE_SESSION_REQ));
		PROTOCOL::SM_CLOSE_SESSION_REQ* pCommand = static_cast<PROTOCOL::SM_CLOSE_SESSION_REQ*>(pPacket->Body());

		pCommand->SSNID = SSNID;
		pCommand->Mode = 0;

		GLOBAL::SendQue.Push(INVALID_SSNID, pPacket);	// 시스템 메시지 전송 (TLNET <== 게임 로직)

		OnCloseSession(SSNID, 0);
	}
}