// Connector.cpp : CConnector 클래스 구현
//

#include "pch.h"
#include "Connector.h"

#include "Global.h"
#include "Session.h"
#include "SessionManager.h"
#include "NetCore.h"

// 네임스페이스 선언
namespace NET
{
	// 생성자/소멸자 구현
	CConnector::CConnector(INT8 type, INT16 _queue_index)
		: m_Type(type)
		, m_queue_index(_queue_index)
	{
	}

	CConnector::~CConnector()
	{
	}

	void CConnector::_connect()
	{
		// 연결 시도
		std::shared_ptr<CSession> pSession = _connect_session();

		if (pSession)
		{
			// 연결 성공
			_start_session(pSession);
			return;
		}

		// 연결 실패 (로직 스레드로 연결이 실패 했음을 알림)
		GLOBAL::NetCore.Send_FAIL_CONNECT_REQ(m_Type);
	}

	std::shared_ptr<CSession> CConnector::_connect_session()
	{
		boost::system::error_code	error;
		std::shared_ptr<CSession>	pSession = GLOBAL::SessionMgr.make_ptr(m_Type, m_queue_index);

		// Boost.Asio 에서 연결 시도 (동기 처리)
		pSession->Socket().connect(m_endpoint, error);

		if (error)
		{
			boost::asio::ip::address _addr = m_endpoint.address();

			size_t cn;

			WCHAR _ip_addr[64] = { 0, };

			mbstowcs_s(&cn, _ip_addr, _addr.to_string().c_str(), _countof(_ip_addr));

			printf("Connect failed (Type: %d, IP: %s, Port: %d)\n", m_Type, _ip_addr, m_endpoint.port());

			return NULL;
		}

		return pSession;
	}

	// 연결이 성공한 경우 처리
	bool CConnector::_start_session(std::shared_ptr<CSession> pSession)
	{
		if (!pSession)
		{
			return false;
		}

		if (INVALID_SSNID == GLOBAL::SessionMgr.Insert(pSession))
		{
			// 오류 처리 : 세션 관리자에 등록 실패
			// 연결 실패 (로직 스레드로 연결이 실패 했음을 알림)
			GLOBAL::NetCore.Send_FAIL_CONNECT_REQ(m_Type);

			printf("Connector Type: %d\n", m_Type);
			return false;
		}

		// 로직 쓰레드로 연결 성공을 알림 (세션 정보 전송)
		GLOBAL::NetCore.Send_OPEN_SESSION_REQ(pSession);

		//----------------------------------------------------------------------

		// 연결된 세션을 패킷 수신 대기 시킴
		pSession->_recv();


		printf("New session connected (Type: %d, SSNID: %d)\n", m_Type, pSession->SSNID());
		return true;
	}

	// 멤버 함수 구현 (가상 함수 재정의)

	// 현재 Connector 객체에서 생성된 Session 객체의 연결이 끊어질 경우
	// 자동으로 다시 연결을 시도
	void CConnector::OnCloseSession()
	{
		if (GLOBAL::NetCore.IsRun())
		{
			_connect();
		}
	}

	bool CConnector::_open(const boost::asio::ip::tcp::endpoint& endpoint)
	{
		m_endpoint = endpoint;

		return true;
	}
}