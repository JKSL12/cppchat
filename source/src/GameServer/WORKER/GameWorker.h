#pragma once

#include "Net/Global.h"
#include "Net/NetWorker.h"

class CGameWorker : public NET::CNetWorker
{
public:
	CGameWorker(CGameServer& parent);
	virtual ~CGameWorker();

	void Send_SSNID(INT16 _ssnid, NET::CPacket* _packet);
	NET::eDISPATCH_RESULT Send_USER(const CUser* pUser, NET::CPacket* pPacket);
	void Send_ALL(NET::CPacket* pPacket);

private:
	CGameServer& m_parent;

public:


private:

#include "GameWorker_GAME.h"

	NET::eDISPATCH_RESULT _on_dispatch(INT16 _ssnid, NET::HEADER* _packet, ULONGLONG arrivedTick);
	

	virtual NET::eDISPATCH_RESULT OnDispatch(INT16 _ssnid, NET::CPacket* _packet, ULONGLONG arrivedTick) override;
	virtual NET::eDISPATCH_RESULT OnOpenSession(INT8 _type, INT16 _ssnid, const TCHAR* _ip_addr, UINT16 _port) override;
	virtual NET::eDISPATCH_RESULT OnCloseSession(INT16 _ssnid, INT8 _mode) override;
	virtual NET::eDISPATCH_RESULT OnFailConnect(INT8 _type) override;
};

