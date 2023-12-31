#include "pch.h"
#include "GameDB.h"

int CGameDB::Open(SERVER::DATABASE_INFO db_info)
{
	return m_Database.Open(db_info.Host, db_info.User, db_info.PWD, db_info.DB);
}