#pragma once

namespace NET
{
	class CSession;
	class CRecvQueue;
	class CSendQueue;
	class CSessionManager;
	class CNetCore;

	namespace GLOBAL
	{
		extern boost::asio::io_context		Io_Context;

		extern CRecvQueue					RecvQue;
		extern CSendQueue					SendQue;
		extern CSessionManager				SessionMgr;
		extern CNetCore						NetCore;
	}
}
