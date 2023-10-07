#pragma once

class CGameClient;

class CClientWorker
{
public:
	CClientWorker(CGameClient& parent);
	~CClientWorker();

	void Run();

	NET::eDISPATCH_RESULT _on_dispatch_game(NET::HEADER* _packet);
	NET::eDISPATCH_RESULT OnDispatch_GAME(NET::HEADER* _packet);

	NET::eDISPATCH_RESULT _on_dispatch(NET::HEADER* _packet, ULONGLONG arrivedTick);

	NET::eDISPATCH_RESULT OnRecv_GS_LOGIN_RES(PROTOCOL::GS_LOGIN_RES* _res);
	NET::eDISPATCH_RESULT OnRecv_GS_CHAT_RES(PROTOCOL::GS_CHAT_RES* _res);
	NET::eDISPATCH_RESULT OnRecv_GS_ENTERCHAT_NFY(PROTOCOL::GS_ENTERCHAT_NFY* _res);
	NET::eDISPATCH_RESULT OnRecv_GS_LEAVECHAT_NFY(PROTOCOL::GS_LEAVECHAT_NFY* _res);

private:
	CGameClient& m_parent;
};

