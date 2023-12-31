#pragma once

#include "Database.h"

namespace DB
{
	class CQuery
	{
	public:
		enum eTYPE
		{
			TYPE_RESULT_SET,	// data를 가져오는 쿼리(select)
			TYPE_COMMAND,		// data를 세팅하는 쿼리(insert, update, delete)

		}; // enum eTYPE
	public:
		CQuery(eTYPE _type);
		CQuery();
		~CQuery();		

		int Create(SQLHDBC hDBC);
		void Release();
		
		// set
		SQLRETURN	BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, UINT8& u8Param);
		SQLRETURN	BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, INT16& i16Param);
		SQLRETURN	BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, INT32& i32Param);
		SQLRETURN	BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, INT64& i64Param);
		SQLRETURN	BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, FLOAT& fParam);
		SQLRETURN	BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, DBTIMESTAMP& tmParam);
		SQLRETURN	BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, TIMESTAMP_STRUCT& tmParam);
		SQLRETURN	BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, SQLTCHAR* strParam, SQLINTEGER nParamLen);

		// get
		SQLRETURN	BindCol(SQLUSMALLINT nCol, UINT8& u8Column);
		SQLRETURN	BindCol(SQLUSMALLINT nCol, INT16& i16Column);
		SQLRETURN	BindCol(SQLUSMALLINT nCol, INT32& i32Column);
		SQLRETURN	BindCol(SQLUSMALLINT nCol, INT64& i64Column);
		SQLRETURN	BindCol(SQLUSMALLINT nCol, FLOAT& fColumn);
		SQLRETURN	BindCol(SQLUSMALLINT nCol, TIMESTAMP_STRUCT& tmColumn);
		SQLRETURN	BindCol(SQLUSMALLINT nCol, DBTIMESTAMP& tmColumn);
		SQLRETURN	BindCol(SQLUSMALLINT nCol, SQLTCHAR* strColumn, SQLINTEGER nColLen);

		int Query(SQLTCHAR* szSqlState = nullptr);
		int FetchRow(SQLTCHAR* szSqlState = nullptr);
		int NextResult(SQLTCHAR* szSqlState = nullptr);
		void Clear(ULONGLONG _start = 0);

		void SetQueryString(SQLWCHAR* sp) { m_sp = sp; }

	private:
		SQLHSTMT m_hSTMT;
		SQLWCHAR* m_sp;

		SQLLEN		m_nParamLen[QUERY_PARAMETER_MAX];

		SQLLEN		m_nAffectedCount;
		eTYPE	m_nType;
	};
}

