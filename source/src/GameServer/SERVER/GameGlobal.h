#pragma once


class CGameServer;
class CGameWorker;
class CUser;

namespace GLOBAL
{
	const UINT16	SERVER_SESSION_MAX = 5;
	const INT16		SYSTEM_DISPATCH_MAX = 1;
	const INT16		USER_DISPATCH_MAX = 1;
	const UINT16	IO_THREAD_MAX = 2;
	const UINT16	SESSION_MAX = 3000;

	enum eDISPATCH_TYPE
	{
		DISPATCH_USER = 1,
		//DISPATCH_DBA = 2,
		//DISPATCH_GM = 3,

		DISPATCH_TYPE_MAX

	};

	extern CGameWorker* GAME_WORKER;
}