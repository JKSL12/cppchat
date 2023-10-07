#pragma once

namespace NET
{
	class CSession;

	class CAcceptor
	{
	public:
		CAcceptor(
			INT8 type = USER_SESSION,
			INT16 _queue_index = USER_QUEUE,
			bool bEncrypt = false);
		virtual ~CAcceptor();

		void Accept();

		bool IsListen();

		bool Open(const boost::asio::ip::tcp::endpoint& endpoint);

		INT8 Type() { return m_Type; }
		INT16 QueueIndex() { return m_queue_index; }

		virtual void Close();
		virtual void OnCloseSession();

	protected:
		INT8	m_Type;				// ���� Ÿ�� (���������� ����)
		INT16	m_queue_index;		// Recv Queue �� �ε���

	private:
		virtual bool _open(const boost::asio::ip::tcp::endpoint& endpoint);

		virtual UINT16 SessionMax();
		virtual UINT16 WaitAcceptMax();

		void _accept_handler(std::shared_ptr<CSession> pSession, const boost::system::error_code& error);

		boost::asio::ip::tcp::acceptor		m_acceptor;			// Boost.Asio �� Acceptor ��ü (�񵿱�)

		bool			m_bEncrypt;			// ������ ���� �� ��ȣȭ ���� ����
		bool			m_bAccept;

		volatile SHORT	m_numListening;		// Accept �� ���ÿ� �޴� ����
		volatile SHORT	m_numEstablished;	// ���� ����Ǿ� �ִ� ��Ʈ��ũ Ŀ�ؼ� ����
	};

}