#include "pch.h"
#include "GameDBProcess.h"

CGameDBProcess* CGameDBProcess::_instance = nullptr;
CGameDBProcess* CGameDBProcess::Instance()
{
	if (_instance == nullptr)
	{
		_instance = new CGameDBProcess();
	}
	return _instance;
}

CGameDBProcess::CGameDBProcess() : s_pGDB(nullptr)
{
}

bool CGameDBProcess::Init(SERVER::DATABASE_INFO db_info)
{
	if (s_pGDB)
	{
		printf("이미 %s DB 객체가 존재합니다\n", db_info.DB);
		return false;
	}

	s_pGDB = new CGameDB;

	if (s_pGDB->Open(db_info) != 0)
	{
		printf("%s 데이터베이스 연결 실패\n", db_info.DB);
		return false;
	}

	return true;
}

void CGameDBProcess::PostQueryTest()
{
	auto query = s_pGDB->ReadQuery();
	query->SetQueryString(const_cast<SQLWCHAR*>(_T("{CALL dbo.P_Test_Get(?,?)}")));
	INT64 param1 = 63, param2 = 0;
	query->BindParam(1, SQL_PARAM_INPUT, param1);
	query->BindParam(2, SQL_PARAM_INPUT, param2);
	query->Query();

	INT64 col1 = 0;
	INT64 col2 = 0;
	UINT8 col3 = 0;


	if (query->FetchRow() != 2)
	{		
		query->BindCol(1, col1);
		query->BindCol(2, col2);
		query->BindCol(3, col3);
	}

	printf("[PostQueryTest] %lld, %lld, %d\n", col1, col2, col3);
}