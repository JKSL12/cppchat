// Connector.cpp : CConnector Ŭ���� ����
//

#include "pch.h"
#include "Connector.h"

#include "Global.h"
#include "Session.h"
#include "SessionManager.h"
#include "NetCore.h"

// ���ӽ����̽� ����
namespace NET
{
	// ������/�Ҹ��� ����
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
		// ���� �õ�
		std::shared_ptr<CSession> pSession = _connect_session();

		if (pSession)
		{
			// ���� ����
			_start_session(pSession);
			return;
		}

		// ���� ���� (���� ������� ������ ���� ������ �˸�)
		GLOBAL::NetCore.Send_FAIL_CONNECT_REQ(m_Type);
	}

	std::shared_ptr<CSession> CConnector::_connect_session()
	{
		boost::system::error_code	error;
		std::shared_ptr<CSession>	pSession = GLOBAL::SessionMgr.make_ptr(m_Type, m_queue_index);

		// Boost.Asio ���� ���� �õ� (���� ó��)
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

	// ������ ������ ��� ó��
	bool CConnector::_start_session(std::shared_ptr<CSession> pSession)
	{
		if (!pSession)
		{
			return false;
		}

		if (INVALID_SSNID == GLOBAL::SessionMgr.Insert(pSession))
		{
			// ���� ó�� : ���� �����ڿ� ��� ����
			// ���� ���� (���� ������� ������ ���� ������ �˸�)
			GLOBAL::NetCore.Send_FAIL_CONNECT_REQ(m_Type);

			printf("Connector Type: %d\n", m_Type);
			return false;
		}

		// ���� ������� ���� ������ �˸� (���� ���� ����)
		GLOBAL::NetCore.Send_OPEN_SESSION_REQ(pSession);

		//----------------------------------------------------------------------

		// ����� ������ ��Ŷ ���� ��� ��Ŵ
		pSession->_recv();


		printf("New session connected (Type: %d, SSNID: %d)\n", m_Type, pSession->SSNID());
		return true;
	}

	// ��� �Լ� ���� (���� �Լ� ������)

	// ���� Connector ��ü���� ������ Session ��ü�� ������ ������ ���
	// �ڵ����� �ٽ� ������ �õ�
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