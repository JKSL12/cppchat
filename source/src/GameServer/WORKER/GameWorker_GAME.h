// GameWorker_GAME.h : Game 관련 프로토콜 처리 함수 선언
//

public:
	NET::eDISPATCH_RESULT OnDispatch_GAME(INT16 _ssnid, NET::HEADER * _packet);

protected:
	NET::eDISPATCH_RESULT _on_dispatch_game(CUser * _user, NET::HEADER * _packet);

public:
	NET::eDISPATCH_RESULT OnRecv_GS_LOGIN_REQ(INT16 _ssnid, PROTOCOL::GS_LOGIN_REQ * _req);
	NET::eDISPATCH_RESULT OnRecv_GS_KEEP_ALIVE_NFY(INT16 _ssnid, PROTOCOL::GS_KEEP_ALIVE_NFY* _req);
	NET::eDISPATCH_RESULT OnRecv_GS_CHAT_REQ(CUser* _user, PROTOCOL::GS_CHAT_REQ* _req);
