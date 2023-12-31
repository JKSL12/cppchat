// SERVER_Types.h : SERVER 관련 상수, 데이터형 선언
//

#pragma once

namespace SERVER
{
	enum eSERVER_STATE : INT8
	{
		STATE_INIT = 0,
		STATE_READY,
		STATE_SYNCH,
		STATE_RUNNING,
		STATE_STOP
	};

	struct DATABASE_INFO
	{
		TCHAR	Host[24];
		TCHAR	User[17];
		TCHAR	PWD[17];
		TCHAR	DB[17];
	};
}

enum eSERVER_RESULT : INT8
{
	SERVER_OK = 0,
	SERVER_FAIL
};