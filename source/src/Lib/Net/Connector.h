#pragma once

namespace NET
{
	class CSession;

	class CConnector
	{
	public:
		CConnector( INT8 type, INT16 _queue_index );
		virtual ~CConnector();

	private:
		void _connect();

	protected:
		std::shared_ptr<CSession> _connect_session();
		bool _start_session(std::shared_ptr<CSession> pSession );

	public:
		virtual void OnCloseSession();

	private:
		virtual bool _open( const boost::asio::ip::tcp::endpoint& endpoint );

		boost::asio::ip::tcp::endpoint	m_endpoint;	

		INT8	m_Type;	
		INT16	m_queue_index;
	};
}