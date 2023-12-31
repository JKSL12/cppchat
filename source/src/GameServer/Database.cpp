#include "pch.h"
#include "Database.h"

namespace DB
{
	SQLHENV			CDatabase::s_hENV = SQL_NULL_HENV;
	std::mutex CDatabase::mMutex;

	int CDatabase::Open(const SQLTCHAR* szHost, const SQLTCHAR* szUser, const SQLTCHAR* szPWD, const SQLTCHAR* szDatabase)
	{
		auto nResult = SQL_ERROR;

		std::lock_guard<std::mutex> lockGuard(CDatabase::mMutex);

		nResult = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &s_hENV);
		if (!SQL_SUCCEEDED(nResult))
		{
			// TODO: 에러 발생.
			return 10;
		}

		nResult = ::SQLSetEnvAttr(s_hENV, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), SQL_IS_INTEGER);
		if (!SQL_SUCCEEDED(nResult))
		{
			return 10;
		}

		// ODBC -> MSSQL 연결(채널별로 여러개 존재 가능)
		nResult = ::SQLAllocHandle(SQL_HANDLE_DBC, s_hENV, &m_hDBC);

		if (!SQL_SUCCEEDED(nResult))
		{
			// TODO: 에러 발생.
			return 10;
		}

		SQLTCHAR	szDBConn[DB_CONNECT_STR_LEN + 1] = { 0, };
		SQLSMALLINT nDBConnLen = 0;
		SQLSMALLINT	nLen = 0;

		TCHAR szHostName[65] = { 0, };
		DWORD dwLen = _countof(szHostName);

		::GetComputerName(szHostName, &dwLen);

		// APP 이하는 해당 로그가 어떤 서버에서 발생했는지 알기 위함
		::StringCchPrintf(szDBConn, DB_CONNECT_STR_LEN, _T("DRIVER={SQL Server Native Client 11.0};SERVER=%s;UID=%s;PWD=%s;DATABASE=%s;"),
			szHost, szUser, szPWD, szDatabase);
		size_t stConnLen = 0;
		::StringCchLength(szDBConn, DB_CONNECT_STR_LEN, &stConnLen);
		nDBConnLen = (SQLSMALLINT)stConnLen;

		nResult = ::SQLDriverConnect(m_hDBC, NULL, szDBConn, nDBConnLen, m_strDBConn, DB_CONNECT_STR_LEN, &nLen, SQL_DRIVER_NOPROMPT);
		if (!SQL_SUCCEEDED(nResult))
		{
			::StringCchPrintf(szDBConn, DB_CONNECT_STR_LEN, _T("DRIVER={ODBC Driver 11 for SQL Server};SERVER=%s;UID=%s;PWD=%s;DATABASE=%s;"),
				szHost, szUser, szPWD, szDatabase);
			size_t stConnLen2 = 0;
			::StringCchLength(szDBConn, DB_CONNECT_STR_LEN, &stConnLen2);
			nDBConnLen = (SQLSMALLINT)stConnLen2;

			nResult = ::SQLDriverConnect(m_hDBC, NULL, szDBConn, nDBConnLen, m_strDBConn, DB_CONNECT_STR_LEN, &nLen, SQL_DRIVER_NOPROMPT);
			if (!SQL_SUCCEEDED(nResult))
			{
				return 10;
			}
		}

		closed_ = false;
		return 0;
	}

	void CDatabase::Close()
	{
		if (SQL_NULL_HDBC != m_hDBC)
		{
			// 채널에 연결된 핸들 날림
			::SQLDisconnect(m_hDBC);
			::SQLFreeHandle(SQL_HANDLE_DBC, m_hDBC);
			m_hDBC = SQL_NULL_HDBC;
		}

		// s_hENV과 s_nRefCount_hENV가 atomic하게 처리되지 않을 수 있으므로 락을 건다.
		{
			std::lock_guard<std::mutex> lockGuard(CDatabase::mMutex);

			if (SQL_NULL_HENV != s_hENV)
			{
				// DBA -> ODBC 연결된 핸들도 날림
				::SQLFreeHandle(SQL_HANDLE_ENV, s_hENV);
				s_hENV = SQL_NULL_HENV;
			}
		}
		closed_ = true;
	}
}