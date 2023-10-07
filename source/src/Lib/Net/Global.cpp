#include "pch.h"
#include "Global.h"

#include "RecvQueue.h"
#include "SendQueue.h"
#include "SessionManager.h"
#include "NetCore.h"

namespace NET
{
	namespace GLOBAL
	{
		boost::asio::io_context		Io_Context;

		CRecvQueue					RecvQue;
		CSendQueue					SendQue;

		CSessionManager				SessionMgr;
		CNetCore					NetCore;
	}
}