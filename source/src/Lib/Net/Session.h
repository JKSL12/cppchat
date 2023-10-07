#pragma once

namespace NET
{
	class CPacket;

	class CSession : public std::enable_shared_from_this<CSession>
	{
	public:
		CSession(
			INT8  type = USER_SESSION,
			INT16 _queue_index = USER_QUEUE,
			bool  bEncrypt = false);
		~CSession();

		void		Activate(INT16 SSNID) { m_SSNID = SSNID; }

		INT8		Type() const { return m_Type; }
		INT16		SSNID() const { return m_SSNID; }
		INT16		QUEUE_INDEX() const { return m_queue_index; }

		boost::asio::ip::tcp::socket& Socket() { return m_Sock; }
		bool		IsClose() const { return m_bClosing; }

		boost::asio::ip::tcp::endpoint	RemotePoint() const;

		void		_close_handler();
		bool		_close();
		void		_recv();
		void		_send(CPacket* pPacket);
		bool		_next_send();

		void		SetSockOption();

	private:
		void		_asio_recv();
		void		_asio_send(CPacket* pPacket);
		void		_recv_body(CPacket* pPacket, const boost::system::error_code& error, std::size_t bytes_recv);
		void		_recv_complete(CPacket* pPacket, const boost::system::error_code& error, std::size_t bytes_recv);
		void		_send_complete(CPacket* pPacket, const boost::system::error_code& error, std::size_t bytes_sent);
		void		_destroy();
		inline void ReceiveCloseDelete(CPacket* const packet);
		void SendClose() { _close_handler(); _close(); }
		bool ErrorCodeFiltering(const int n) { return ERROR_FILE_NOT_FOUND != n && ERROR_SEM_TIMEOUT != n && ERROR_CONNECTION_ABORTED != n && WSAECONNABORTED != n && WSAECONNRESET != n; }

		INT8			m_Type;
		INT16			m_SSNID;
		INT16			m_queue_index;

		boost::asio::ip::tcp::socket			m_Sock;
		boost::asio::io_context::strand			m_recvStrand;		
		boost::asio::io_context::strand			m_sendStrand;

		bool			m_bEncrypt;

		INT32           m_sec_recvpacket_cnt;
		ULONGLONG       m_sec_recvtime;

		boost::shared_mutex m_lock;

		volatile bool	m_bClosing;
		volatile UINT8	m_IoFlag;

		std::deque<CPacket *>	m_queSend;
	};

}