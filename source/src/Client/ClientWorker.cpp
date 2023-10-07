#include "pch.h"
#include "ClientWorker.h"

#include "Net/Global.h"
#include "Net/Packet.h"
#include "Net/RecvQueue.h"
#include "Net/NetCore.h"
#include "GameClient.h"

CClientWorker::CClientWorker(CGameClient& parent)
	: m_parent(parent)
{

}

CClientWorker::~CClientWorker()
{

}

void CClientWorker::Run()
{
	while (NET::GLOBAL::NetCore.IsRun())
	{
		INT16	  _ssnid = NET::INVALID_SSNID;
		NET::CPacket* _packet = NULL;
		ULONGLONG _tick = 0;
		bool needSleep = true;

		INT16 _dispatch_max = NET::GLOBAL::RecvQue.GetDispatchMax(NET::USER_QUEUE);

		for (int i = 0; (_dispatch_max > i) && (NET::GLOBAL::NetCore.IsRun()); ++i)
		{
			if (NET::GLOBAL::RecvQue.Pop(NET::USER_QUEUE, _ssnid, &_packet, _tick))
			{
				if (NET::DISPATCH_OK != _on_dispatch(_packet->Header(), _tick))
				{
					//CloseSession(_ssnid);
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

		m_parent.UpdateRoutines();						// 업데이트 루틴

		if (needSleep)
		{
			::Sleep(1);
		}
	}
}

NET::eDISPATCH_RESULT CClientWorker::_on_dispatch(NET::HEADER* _packet, ULONGLONG arrivedTick)
{
	NET::eDISPATCH_RESULT _result = NET::eDISPATCH_RESULT::DISPATCH_FAIL;

	switch (PROTOCOL::GET_PROTOCOL_ID(_packet->type))
	{
	case PROTOCOL::ID::PROTOCOL_GAME:		_result = OnDispatch_GAME(_packet);		break;

	default:
		_result = NET::eDISPATCH_RESULT::DISPATCH_INVALID_PACKET;
		printf("[PACKET] Packet ID: 0x%08X [%d]\n",
			_packet->type, _result);
	}

	return _result;
}