#pragma once

#include "Database.h"
#include "Query.h"

namespace DB
{
	class CDBManager
	{
	public:
		virtual int Open(SERVER::DATABASE_INFO db_info) = 0;
		std::shared_ptr<CQuery> ReadQuery();
		std::shared_ptr<CQuery> WriteQuery();

	private:
		std::shared_ptr<CQuery> Create(CQuery::eTYPE _type);

	protected:
		DB::CDatabase			m_Database;
	};
}

