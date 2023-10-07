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
		INT8	m_Type;				// 연결 타입 (컨텐츠에서 결정)
		INT16	m_queue_index;		// Recv Queue 의 인덱스

	private:
		virtual bool _open(const boost::asio::ip::tcp::endpoint& endpoint);

		virtual UINT16 SessionMax();
		virtual UINT16 WaitAcceptMax();

		void _accept_handler(std::shared_ptr<CSession> pSession, const boost::system::error_code& error);

		boost::asio::ip::tcp::acceptor		m_acceptor;			// Boost.Asio 의 Acceptor 객체 (비동기)

		bool			m_bEncrypt;			// 연결을 받을 때 암호화 여부 지정
		bool			m_bAccept;

		volatile SHORT	m_numListening;		// Accept 를 동시에 받는 개수
		volatile SHORT	m_numEstablished;	// 현재 연결되어 있는 네트워크 커넥션 개수
	};

}