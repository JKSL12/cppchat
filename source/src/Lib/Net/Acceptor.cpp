#include "pch.h"
#include "Acceptor.h"

#include "Global.h"
#include "Session.h"
#include "NetCore.h"
#include "SessionManager.h"

namespace NET
{
	CAcceptor::CAcceptor(
		INT8 type /*= USER_SESSION*/,
		INT16 _queue_index /*= USER_QUEUE*/,
		bool bEncrypt /*= false*/
	)
		: m_Type(type)
		, m_queue_index(_queue_index)
		, m_acceptor(GLOBAL::Io_Context)
		, m_bEncrypt(bEncrypt)
		, m_bAccept(true)
		, m_numListening(0)
		, m_numEstablished(0)
	{
	}

	CAcceptor::~CAcceptor()
	{
		Close();
	}
	
	void CAcceptor::Accept()
	{
		if (GLOBAL::NetCore.IsRun())
		{
			while (IsListen() && m_bAccept)
			{
				// 연결을 받을 객체를 미리 생성하는 코드
				std::shared_ptr<CSession> pSession = GLOBAL::SessionMgr.make_ptr(m_Type, m_queue_index, m_bEncrypt);

				// 연결 대기 (클라이언트로 부터 연결 요청을 받으면 _accept_handler 함수 호출)
				m_acceptor.async_accept(pSession->Socket(),
					boost::bind(&CAcceptor::_accept_handler, this, pSession, boost::asio::placeholders::error));

				InterlockedIncrement16(&m_numListening);
			}
		}
	}

	bool CAcceptor::IsListen()
	{
		return (((m_numListening + m_numEstablished) < (SESSION_MAX + 1))
			&& (m_numListening < WAIT_ACCEPT_MAX));
	}

	bool CAcceptor::Open(const boost::asio::ip::tcp::endpoint& endpoint)
	{
		boost::system::error_code error;

		m_acceptor.open(endpoint.protocol(), error);
		if(error) return false;

		m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), error);
		if (error) return false;

		m_acceptor.bind(endpoint, error);
		if (error) return false;

		m_acceptor.listen(boost::asio::ip::tcp::acceptor::max_connections, error);
		if (error) return false;

		return true;
	}

	void CAcceptor::Close()
	{
		if (m_acceptor.is_open())
		{
			m_acceptor.close();
		}
	}

	void CAcceptor::OnCloseSession()
	{
		InterlockedDecrement16(&m_numEstablished);

		if (GLOBAL::NetCore.IsRun())
		{
			Accept();
		}
	}

	
	bool CAcceptor::_open(const boost::asio::ip::tcp::endpoint& endpoint)
	{
		boost::system::error_code error;

		m_acceptor.open(endpoint.protocol(), error);
		if(error)	return false;

		m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), error);
		if (error)	return false;

		m_acceptor.bind(endpoint, error);
		if (error)	return false;

		m_acceptor.listen(boost::asio::ip::tcp::acceptor::max_connections, error);
		if (error)	return false;

		return true;
	}

	UINT16 CAcceptor::SessionMax()
	{
		return SESSION_MAX;
	}

	UINT16 CAcceptor::WaitAcceptMax()
	{
		return WAIT_ACCEPT_MAX;
	}

	void CAcceptor::_accept_handler(std::shared_ptr<CSession> pSession, const boost::system::error_code& error)
	{
		InterlockedDecrement16(&m_numListening);

		if (error)
		{
			// 오류 처리 : 네트워크 오류 발생
			if (!GLOBAL::NetCore.IsRun())
			{
				return;
			}

			printf("%s", error.message().c_str());
		}
		else if ((SESSION_MAX <= m_numEstablished) || (INVALID_SSNID == GLOBAL::SessionMgr.Insert(pSession)))
		{

		}
		else
		{
			//	TODO: 최대하
			//	소켓옵션 추가
			pSession->SetSockOption();

			// 로직 쓰레드로 세션이 열렸음을 알림
			GLOBAL::NetCore.Send_OPEN_SESSION_REQ(pSession);

			//------------------------------------------------------------------

			// 연결된 세션을 패킷 수신 대기 시킴
			pSession->_recv();

			InterlockedIncrement16(&m_numEstablished);
		}

		// 다음 연결을 받을 객체를 생성
		Accept();
	}
}