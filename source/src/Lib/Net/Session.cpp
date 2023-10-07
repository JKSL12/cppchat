#include "pch.h"
#include "Session.h"

#include "Global.h"
#include "RecvQueue.h"
#include "SessionManager.h"
#include "Packet.h"
#include "NetCore.h"

namespace NET
{
	CSession::CSession(
		INT8 type, // USER_SESSION
		INT16 _queue_index, // USER_QUEUE
		bool bEncrypt // false
	)
		: m_Type(type)
		, m_SSNID(INVALID_SSNID)
		, m_queue_index(_queue_index)
		, m_Sock(GLOBAL::Io_Context)
		, m_recvStrand(GLOBAL::Io_Context)	
		, m_sendStrand(GLOBAL::Io_Context)
		, m_bEncrypt(bEncrypt)
		, m_bClosing(false)
		, m_IoFlag(IO_FLAG_NONE)
		, m_sec_recvpacket_cnt(0)
		, m_sec_recvtime(0)
	{
		
	}

	CSession::~CSession()
	{
		_close();
		_destroy();
	}

	boost::asio::ip::tcp::endpoint CSession::RemotePoint() const
	{
		return m_Sock.remote_endpoint();
	}

	bool CSession::_close()
	{
		boost::unique_lock<boost::shared_mutex>(m_lock);

		m_bClosing = true;

		boost::system::error_code errCode;
		m_Sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, errCode);
		m_Sock.close();

		m_IoFlag = IO_FLAG_NONE;

		GLOBAL::SessionMgr.Remove(m_Type, m_SSNID);
		return true;
	}

	void CSession::_close_handler()
	{
		boost::unique_lock<boost::shared_mutex>(m_lock);

		if (true == m_bClosing)
			return;

		GLOBAL::NetCore.Send_CLOSE_SESSION_REQ(shared_from_this());
		/*CCnntObj* pCnntObj = GLOBAL::CnntObjMngr.Find(m_Type);
		if (pCnntObj)
		{
			pCnntObj->_close_handler();
		}*/
	}

	void CSession::_recv()
	{
		boost::unique_lock<boost::shared_mutex>(m_lock);

		m_IoFlag |= IO_FLAG_RECV;
		_asio_recv();
	}

	void CSession::_send(CPacket* pPacket)
	{
		boost::unique_lock<boost::shared_mutex>(m_lock);

		if (IsClose())
		{
			return;
		}
		else if (m_IoFlag & IO_FLAG_SEND)
		{
			m_queSend.push_back(pPacket);
		}
		else
		{
			m_IoFlag |= IO_FLAG_SEND;
			_asio_send(pPacket);
		}
	}

	bool CSession::_next_send()
	{
		boost::unique_lock<boost::shared_mutex>(m_lock);

		if (IsClose())
		{
			return false;
		}
		else if (m_queSend.empty())
		{
			m_IoFlag &= ~IO_FLAG_SEND;
		}
		else
		{
			_asio_send(m_queSend.front());
			m_queSend.pop_front();
		}

		return true;
	}

	void CSession::_asio_recv()
	{
		CPacket* pPacket = new CPacket();
		
		boost::asio::async_read(m_Sock, boost::asio::buffer(pPacket->Header(), HEADER_SIZE), boost::bind(&CSession::_recv_body, shared_from_this(), pPacket, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	void CSession::_asio_send(CPacket* pPacket)
	{
		auto size = pPacket->PACKET_SIZE();
		(void)size;
		boost::asio::async_write(m_Sock, boost::asio::buffer(pPacket->Data(), pPacket->PACKET_SIZE()), boost::bind(&CSession::_send_complete, shared_from_this(), pPacket, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	void CSession::_recv_body(CPacket* pPacket, const boost::system::error_code& error, std::size_t bytes_recv)
	{
		if (error)
		{
			int _code = error.value();

			// 서버가 강제로 연결을 끊었는데 클라이언트에서 온 데이터가 있는 경우에는 ERROR_OPERATION_ABORTED 에러가 발생한다. 서버가 강제로 연결을 끊은 경우에만 해당 에러에 대한 로그를 남기지 않으면 좋겠지만, 그러기에는 코드가 복잡해서 세세하게 처리하기 어려우므로 그냥 연결을 끊는 중이면 해당 에러에 대한 로그를 남기지 않도록 한다.
			if (ErrorCodeFiltering(_code) && (!m_bClosing || m_bClosing && _code != ERROR_OPERATION_ABORTED))
			{
				printf("%s\n", error.message().c_str());
			}

			ReceiveCloseDelete(pPacket);
			return;
		}

		if (HEADER_SIZE != bytes_recv)
		{
			printf("Received packet does not match the size of the header (%d != %zd)\n", HEADER_SIZE, bytes_recv);
			ReceiveCloseDelete(pPacket);
			return;
		}
		else if (PACKET_SIZE_MAX < pPacket->PACKET_SIZE())
		{
			printf("Packet size has been exceeded (%d < %d)\n", PACKET_SIZE_MAX, pPacket->PACKET_SIZE());
			ReceiveCloseDelete(pPacket);
			return;
		}
		else if (0 == pPacket->Size())
		{
			/*if (IsEncrpyt() == true)
			{
				if (pPacket->Decode(GetEncrypt_Recv()) == false)
				{
#if (_NETWORK_LOG_LEVEL<=_NETWORK_LOG_ERROR)
					_ERROR_LOG_FILE("ENCRYPT_ERROR.txt", "Packet decrypt failed (SSNID: %d)", SSNID());
#endif
					ReceiveCloseDelete(pPacket);
					return;
				}
			}*/
			GLOBAL::RecvQue.Push(QUEUE_INDEX(), SSNID(), pPacket);

			_asio_recv();
			return;
		}

		if (pPacket->PACKET_SIZE() > pPacket->DATA_SIZE())
			pPacket->Resize();

		assert(pPacket->Size() <= pPacket->DATA_SIZE());

		boost::asio::async_read(m_Sock, boost::asio::buffer(pPacket->Body(), pPacket->Size()), boost::bind(&CSession::_recv_complete, shared_from_this(), pPacket, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	// 패킷 바디 수신 처리
	void CSession::_recv_complete(CPacket* pPacket, const boost::system::error_code& error, std::size_t bytes_recv)
	{
		if (error)
		{
			int _code = error.value();

			if (ErrorCodeFiltering(_code))
			{
				printf("%s\n", error.message().c_str());
			}

			ReceiveCloseDelete(pPacket);
			return;
		}
		else if (pPacket->Size() != bytes_recv)
		{
			printf("Does not match the size of the data packets received (%d != %zd)\n", pPacket->Size(), bytes_recv);
			ReceiveCloseDelete(pPacket);
			return;
		}

		/*if (IsEncrpyt() == true)
		{
			if (pPacket->Decode(GetEncrypt_Recv()) == false)
			{
#if (_NETWORK_LOG_LEVEL<=_NETWORK_LOG_ERROR)
				_ERROR_LOG_FILE("ENCRYPT_ERROR.txt", "Packet decrypt failed (SSNID: %d)", SSNID());
#endif
				ReceiveCloseDelete(pPacket);
				return;
			}
		}*/

		GLOBAL::RecvQue.Push(QUEUE_INDEX(), SSNID(), pPacket);

		_asio_recv();
	}

	void CSession::_send_complete(CPacket* pPacket, const boost::system::error_code& error, std::size_t bytes_sent)
	{
		INT32 nPacketType = pPacket->Type();
		INT32 nPacketSize = pPacket->PACKET_SIZE();

		delete pPacket;

		if (error)
		{
			int _code = error.value();

			printf("CSession::_send_complete(pPacket, error, bytes_sent) _code : %d, Session Type: %d, SSNID: %d, nPacketType = %d, nPacketSize = %d\n", _code, Type(), SSNID(), nPacketType, nPacketSize);			

			if (ErrorCodeFiltering(_code))
			{
				printf("%s\n", error.message().c_str());
			}

			SendClose();
			return;
		}
		else if (nPacketSize != bytes_sent)
		{
			printf("Does not match the size of the packets sent (%d != %zd)\n", nPacketSize, bytes_sent);

			SendClose();
			return;
		}
		else
		{

		}

		if (!_next_send())
		{
			SendClose();
			return;
		}

		//printf("Send Packet %d\n", nPacketType);
	}

	void CSession::_destroy()
	{
		while (!m_queSend.empty())
		{
			delete m_queSend.front();
			m_queSend.pop_front();
		}
	}

	void CSession::SetSockOption()
	{
		//	SOL_SOCKET / SO_LINGER
		m_Sock.set_option(boost::asio::socket_base::linger(true, 0));

		//	IPPROTO_TCP / TCP_NODELAY
		m_Sock.set_option(boost::asio::ip::tcp::no_delay(true));

		//	SOL_SOCKET / SO_RCVBUF
		m_Sock.set_option(boost::asio::socket_base::receive_buffer_size(CPacket::SIZE_512K));

		//	SOL_SOCKET / SO_SNDBUF
		m_Sock.set_option(boost::asio::socket_base::send_buffer_size(CPacket::SIZE_512K));
	}

	void CSession::ReceiveCloseDelete(CPacket* const packet)
	{
		_close_handler();
		_close();
		delete packet;
	}
}