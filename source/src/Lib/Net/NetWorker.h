#pragma once

#include "GameServer/PROTOCOL/Protocol_SYSTEM.h"

namespace NET
{
	class CPacket;

	class CNetWorker
	{
	public:
		CNetWorker();
		virtual ~CNetWorker();

		void Run_ALL();

		static void Send(INT16 _ssnid, CPacket* _packet);

		void CloseSession(INT16 SSNID);

	private:
		bool _run_dispatch_system();						// 시스템 메시지 Dispatch
		bool _run_on_dispatch(INT16 _queue_index);		// 일반 메시지 Dispatch

		void Dispatch_SYSTEM(INT16 SSNID, CPacket* pPacket);

		eDISPATCH_RESULT OnSM_OPEN_SESSION_REQ(INT16 SSNID, PROTOCOL::SM_OPEN_SESSION_REQ* pPacket);
		eDISPATCH_RESULT OnSM_CLOSE_SESSION_REQ(INT16 SSNID, PROTOCOL::SM_CLOSE_SESSION_REQ* pPacket);
		eDISPATCH_RESULT OnSM_FAIL_CONNECT_REQ(INT16 SSNID, PROTOCOL::SM_FAIL_CONNECT_REQ* pPacket);


		virtual bool OnInitialize() { return true; }
		virtual void OnFinalize() {}

		virtual void UpdateRoutines() { ::Sleep(1); }

		virtual eDISPATCH_RESULT OnDispatch(INT16 SSNID, CPacket* pPacket, ULONGLONG arrivedTick) = 0;
		virtual eDISPATCH_RESULT OnOpenSession(INT8 type, INT16 SSNID, const WCHAR* ip_addr, UINT16 port) = 0;
		virtual eDISPATCH_RESULT OnCloseSession(INT16 SSNID, INT8 Mode) = 0;
		virtual eDISPATCH_RESULT OnFailConnect(INT8 type) = 0;
	};
}

