#include "../pch.h"
#include "GameServer.h"

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
	// ����
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