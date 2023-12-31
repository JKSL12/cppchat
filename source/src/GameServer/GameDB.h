#pragma once

#include "DBManager.h"

class CGameDB : public DB::CDBManager
{
public:
	int Open(SERVER::DATABASE_INFO db_info) override;
};

