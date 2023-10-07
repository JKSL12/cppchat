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
}

enum eSERVER_RESULT : INT8
{
	SERVER_OK = 0,
	SERVER_FAIL
};