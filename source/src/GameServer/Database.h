#pragma once

namespace DB
{
	const int	SQLSTATE_LEN = SQL_SQLSTATE_SIZE + 1;
	const int	DB_CONNECT_STR_LEN = 1024;	
	const int	QUERY_PARAMETER_MAX = 1000;

	class CDatabase
	{
		std::atomic<bool>		closed_ = true;

	public:
		bool IsClosed() { return closed_; }
		int Open(const SQLTCHAR* szHost, const SQLTCHAR* szUser, const SQLTCHAR* szPWD, const SQLTCHAR* szDatabase);
		void Close();

		SQLHDBC	GetHDBC() { return m_hDBC; }
		

	private:
		static SQLHENV		s_hENV;
		static std::mutex	mMutex;

		SQLHDBC		m_hDBC;
		SQLTCHAR	m_strDBConn[DB_CONNECT_STR_LEN + 1];
	};
}