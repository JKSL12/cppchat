#include "../pch.h"
#include "GameServer.h"
#include "RedisCommon.h"
#include "GameDBProcess.h"

#if defined(_DEBUG)
#pragma comment(lib, "hiredisD")
#else
#pragma comment(lib, "hiredis")
#endif

CGameServer::CGameServer()
	: m_bRun(false)
	, m_Worker(*this)
	, m_state(SERVER::STATE_INIT)
	, m_UserAcceptor(NET::USER_QUEUE)
{
	GLOBAL::GAME_WORKER = &m_Worker;
	

	::srand(static_cast<unsigned int>(time(NULL)));
}

CGameServer::~CGameServer()
{
}

eSERVER_RESULT CGameServer::PreInitialize()
{
	// DB
	SERVER::DATABASE_INFO db_info;
	_tcscpy_s(db_info.Host, _countof(db_info.Host), _T("127.0.0.1,0"));
	_tcscpy_s(db_info.User, _countof(db_info.User), _T("user"));
	_tcscpy_s(db_info.PWD, _countof(db_info.PWD), _T("pwd"));
	_tcscpy_s(db_info.DB, _countof(db_info.DB), _T("db"));
	
	if (!CGameDBProcess::Instance()->Init(db_info))
	{
		return SERVER_FAIL;
	}

	CGameDBProcess::Instance()->PostQueryTest();

	// ·¹µð½º
	char charBuff[] = "127.0.0.1";
	TCHAR szHost[256] = { 0, };
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, charBuff, strlen(charBuff), szHost, 256);

	RedisCommon::Instance()->Init(szHost, 6379, const_cast<TCHAR*>(_T("")), 2, 1);

	if (!RedisCommon::Instance()->ConnectionTest())
	{
		return SERVER_FAIL;
	}

	return SERVER_OK;
}

bool CGameServer::OnInitialize()
{
	return true;
}

void CGameServer::OnFinalize()
{
	//GLOBAL::Finalize();
}

bool CGameServer::OnStart()
{
	if (!m_UserAcceptor.Open(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 9000)))
	{
		printf("Open World Server Port for User failed (Port: %d)", 9000);
		return false;
	}

	if (!RegisterAcceptObj(&m_UserAcceptor, GLOBAL::USER_DISPATCH_MAX))
	{
		printf("Register ServerAcceptor failed (Type: %d)\n", m_UserAcceptor.Type());
		return false;
	}

	m_UserAcceptor.Accept();

	m_bRun = true;

	printf("Server Start\n");

	return true;
}

void CGameServer::OnStop()
{
	::Sleep(100);
}

bool CGameServer::OnRun()
{
	return m_bRun;
}

CGameWorker* CGameServer::GetWorker()
{
	m_bRun = true;

	return &m_Worker;
}