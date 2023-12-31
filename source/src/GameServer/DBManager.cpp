#include "pch.h"
#include "DBManager.h"

namespace DB
{
	std::shared_ptr<CQuery> CDBManager::WriteQuery()
	{
		auto query = Create(CQuery::eTYPE::TYPE_COMMAND);

		return query;
	}

	std::shared_ptr<CQuery> CDBManager::ReadQuery()
	{
		auto query = Create(CQuery::eTYPE::TYPE_RESULT_SET);

		return query;
	}

	std::shared_ptr<CQuery> CDBManager::Create(CQuery::eTYPE _type)
	{				
		std::shared_ptr<CQuery> query = std::make_shared<CQuery>();
		
		int nResult = query->Create(m_Database.GetHDBC());
		// 쿼리 하나 당 개별 ODBC 핸들 생성
		
		if (!SQL_SUCCEEDED(nResult))
		{
			return nullptr;
		}

		return query;
	}	
}
//DEFINE_QUERY("{CALL dbo.P_Item_Pakage_Set(?,?,?,?,?,?,?,?,?,?,?,?)}")