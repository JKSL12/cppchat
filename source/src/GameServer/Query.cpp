#include "pch.h"
#include "Query.h"

namespace DB
{
	CQuery::CQuery(eTYPE _type)
		: m_hSTMT(SQL_NULL_HSTMT)
		, m_nType(_type)
		, m_nAffectedCount(0)
	{
	}

	CQuery::CQuery()
	{
	}

	CQuery::~CQuery()
	{
		Release();
	}

	int CQuery::Create(SQLHDBC hDBC)
	{
		SQLRETURN nResult = SQL_ERROR;

		nResult = ::SQLAllocHandle(SQL_HANDLE_STMT, hDBC, &m_hSTMT);

		/*if (!SQL_SUCCEEDED(nResult))
		{
			return nResult;
		}*/

		return nResult;
	}

	void CQuery::Release()
	{
		if (SQL_NULL_HSTMT != m_hSTMT)
		{
			::SQLFreeHandle(SQL_HANDLE_STMT, m_hSTMT);
			m_hSTMT = SQL_NULL_HSTMT;
		}
	}
	
	SQLRETURN CQuery::BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, UINT8& u8Param)
	{
		m_nParamLen[nParam] = 0;
		return ::SQLBindParameter(m_hSTMT, nParam, nType, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &u8Param, 0, &m_nParamLen[nParam]);
	}

	SQLRETURN CQuery::BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, INT16& i16Param)
	{
		m_nParamLen[nParam] = 0;
		return ::SQLBindParameter(m_hSTMT, nParam, nType, SQL_C_SHORT, SQL_SMALLINT, 0, 0, &i16Param, 0, &m_nParamLen[nParam]);
	}

	SQLRETURN CQuery::BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, INT32& i32Param)
	{
		m_nParamLen[nParam] = 0;
		return ::SQLBindParameter(m_hSTMT, nParam, nType, SQL_C_LONG, SQL_INTEGER, 0, 0, &i32Param, 0, &m_nParamLen[nParam]);
	}

	SQLRETURN CQuery::BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, INT64& i64Param)
	{
		m_nParamLen[nParam] = 0;
		return ::SQLBindParameter(m_hSTMT, nParam, nType, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &i64Param, 0, &m_nParamLen[nParam]);
	}

	SQLRETURN CQuery::BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, FLOAT& fParam)
	{
		m_nParamLen[nParam] = 0;
		return ::SQLBindParameter(m_hSTMT, nParam, nType, SQL_C_FLOAT, SQL_REAL, 0, 0, &fParam, 0, &m_nParamLen[nParam]);
	}

	SQLRETURN CQuery::BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, DBTIMESTAMP& tmParam)
	{
		m_nParamLen[nParam] = 0;
		return ::SQLBindParameter(m_hSTMT, nParam, nType, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, SQL_TIMESTAMP_LEN, 3, &tmParam, sizeof(DBTIMESTAMP), &m_nParamLen[nParam]);
	}

	SQLRETURN CQuery::BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, TIMESTAMP_STRUCT& tmParam)
	{
		m_nParamLen[nParam] = 0;
		return ::SQLBindParameter(m_hSTMT, nParam, nType, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, SQL_TIMESTAMP_LEN, 3, &tmParam, sizeof(TIMESTAMP_STRUCT), &m_nParamLen[nParam]);
	}

	SQLRETURN CQuery::BindParam(SQLUSMALLINT nParam, SQLSMALLINT nType, SQLTCHAR* strParam, SQLINTEGER nParamLen)
	{
		m_nParamLen[nParam] = SQL_NTS;
		return ::SQLBindParameter(m_hSTMT, nParam, nType, SQL_C_TCHAR, SQL_WVARCHAR, nParamLen / sizeof(SQLTCHAR), 0, strParam, nParamLen, &m_nParamLen[nParam]);
	}

	SQLRETURN CQuery::BindCol(SQLUSMALLINT nCol, UINT8& u8Column)
	{
		SQLLEN nDataLen;
		return ::SQLGetData(m_hSTMT, nCol, SQL_C_UTINYINT, &u8Column, 0, &nDataLen);
	}

	SQLRETURN CQuery::BindCol(SQLUSMALLINT nCol, INT16& i16Column)
	{
		SQLLEN nDataLen;
		return ::SQLGetData(m_hSTMT, nCol, SQL_C_SHORT, &i16Column, 0, &nDataLen);
	}

	SQLRETURN CQuery::BindCol(SQLUSMALLINT nCol, INT32& i32Column)
	{
		SQLLEN nDataLen;
		return ::SQLGetData(m_hSTMT, nCol, SQL_C_LONG, &i32Column, 0, &nDataLen);
	}

	SQLRETURN CQuery::BindCol(SQLUSMALLINT nCol, INT64& i64Column)
	{
		SQLLEN nDataLen;
		return ::SQLGetData(m_hSTMT, nCol, SQL_C_SBIGINT, &i64Column, 0, &nDataLen);
	}

	SQLRETURN CQuery::BindCol(SQLUSMALLINT nCol, FLOAT& fColumn)
	{
		SQLLEN nDataLen;
		return ::SQLGetData(m_hSTMT, nCol, SQL_C_FLOAT, &fColumn, 0, &nDataLen);
	}

	SQLRETURN CQuery::BindCol(SQLUSMALLINT nCol, DBTIMESTAMP& tmColumn)
	{
		SQLLEN nDataLen;
		return ::SQLGetData(m_hSTMT, nCol, SQL_C_TYPE_TIMESTAMP, &tmColumn, 0, &nDataLen);
	}

	SQLRETURN CQuery::BindCol(SQLUSMALLINT nCol, TIMESTAMP_STRUCT& tmColumn)
	{
		SQLLEN nDataLen;
		return ::SQLGetData(m_hSTMT, nCol, SQL_C_TYPE_TIMESTAMP, &tmColumn, 0, &nDataLen);
	}

	SQLRETURN CQuery::BindCol(SQLUSMALLINT nCol, SQLTCHAR* strColumn, SQLINTEGER nColLen)
	{
		SQLLEN nDataLen;
		return ::SQLGetData(m_hSTMT, nCol, SQL_C_TCHAR, strColumn, nColLen, &nDataLen);
	}

	int CQuery::Query(SQLTCHAR* szSqlState)
	{
		SQLRETURN nResult = SQL_ERROR;

		//----------------------------------------------------------------------

		// ODBC 핸들에 파라메터 매핑
	/*	nResult = InitParameters();
		if (SQL_FAILED(nResult))
		{
			m_nErrorCode = GetError(SQL_HANDLE_STMT, m_hSTMT, GetQueryString(), szSqlState);
			return SQL_E_FAIL;
		}*/

		//----------------------------------------------------------------------

		nResult = ::SQLExecDirect(m_hSTMT, m_sp, SQL_NTS);
		if (SQL_NEED_DATA == nResult)
		{
			return 10;
		}

		thread_local SQLTCHAR beforeQuery[128] = _T("");

		if (!SQL_SUCCEEDED(nResult) && (SQL_NO_DATA != nResult))
		{
			SQLTCHAR sqlState[DB::SQLSTATE_LEN] = _T("");

			//m_nErrorCode = GetError(SQL_HANDLE_STMT, m_hSTMT, GetQueryString(), sqlState);

			if (szSqlState)
			{
				::_tcsncpy_s(szSqlState, sizeof(szSqlState), sqlState, DB::SQLSTATE_LEN);
				szSqlState[DB::SQLSTATE_LEN - 1] = _T('\0');
			}

			/*if (WSAECONNRESET == m_nErrorCode)
				m_nErrorCode = WSAECONNRESET;*/

			if (::_tcsncmp(sqlState, _T("08S01"), DB::SQLSTATE_LEN) == 0)
			{
				//_ERROR_LOG("CQuery::Query(szSqlState) beforeQuery = %s, GetQueryString() = %s", beforeQuery, GetQueryString());
			}
			::_tcsncpy_s(beforeQuery, m_sp, 127);
			beforeQuery[127] = _T('\0');
			return 10;
		}
		::_tcsncpy_s(beforeQuery, m_sp, 127);
		beforeQuery[127] = 0;

		//----------------------------------------------------------------------

		if (TYPE_COMMAND == m_nType)
		{
			nResult = ::SQLRowCount(m_hSTMT, &m_nAffectedCount);
			if (!SQL_SUCCEEDED(nResult))
			{
				//m_nErrorCode = GetError(SQL_HANDLE_STMT, m_hSTMT, GetQueryString(), szSqlState);
			}
		}

		//++m_query_count;

		return static_cast<int>(nResult);
	}

	int CQuery::FetchRow(SQLTCHAR* szSqlState)
	{
		SQLRETURN nResult = ::SQLFetch(m_hSTMT);

		if (SQL_NO_DATA != nResult)
		{
			if (!SQL_SUCCEEDED(nResult))
			{
				//m_nErrorCode = GetError(SQL_HANDLE_STMT, m_hSTMT, GetQueryString(), szSqlState);
				return 10;
			}

			return 0;
		}

		return 2;
	}

	// 쿼리 실행 결과값으로 여러 Result_Set이 있을 때, 다음에 가져올 Result_Set으로 전환
	// DB_NEXT_RESULT 매크로 사용
	int CQuery::NextResult(SQLTCHAR* szSqlState)
	{
		SQLRETURN nResult = ::SQLMoreResults(m_hSTMT);

		if (!SQL_SUCCEEDED(nResult) && (SQL_NO_DATA != nResult))
		{
			//m_nErrorCode = GetError(SQL_HANDLE_STMT, m_hSTMT, GetQueryString(), szSqlState);
			return 10;
		}

		return 0;
	}

	void CQuery::Clear(ULONGLONG _start)
	{
		if (CQuery::TYPE_RESULT_SET == m_nType)
		{
			::SQLCloseCursor(m_hSTMT);
		}

		//m_query_time += (::GetTickCount64() - _start);
	}
}
