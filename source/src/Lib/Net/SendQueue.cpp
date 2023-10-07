#include "pch.h"
#include "SendQueue.h"

#include "Global.h"
#include "Session.h"
#include "SessionManager.h"
#include "NetCore.h"
#include "Packet.h"

namespace NET
{
	CSendQueue::CSendQueue()
	{
	}

	CSendQueue::~CSendQueue()
	{
	}

	void CSendQueue::Push(INT16 SSNID, CPacket* pPacket)
	{
		std::shared_ptr<CSession> pSession = GLOBAL::SessionMgr.Find(SSNID);

		if (pSession)
		{
			pSession->_send(pPacket);
		}
		else
		{
			if (INVALID_SSNID == SSNID)
			{
				//_ERROR_LOG("Dispatch pPacket type : %d size : %d", pPacket->Header()->type, pPacket->Size());
				GLOBAL::NetCore.Dispatch(pPacket);
			}
			else
			{
				//_ERROR_LOG( "INVALID SESSION (SSNID: %d)", SSNID );
			}

			delete pPacket;
		}
	}
}