#pragma once

#include "GameDB.h"

class CGameDBProcess
{
	CGameDBProcess();
	static CGameDBProcess* _instance;

public:
	~CGameDBProcess();

	static CGameDBProcess* Instance();

	bool Init(SERVER::DATABASE_INFO db_info);

	void PostQueryTest();

private:
	CGameDB* s_pGDB;
};