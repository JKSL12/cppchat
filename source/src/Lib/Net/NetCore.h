#pragma once

namespace NET
{
	class CPacket;
	class CSession;

	class CNetCore
	{
	public:
		CNetCore();
		~CNetCore();

		// 기본 멤버 함수
	public:
		void Start();
		void Stop();
		void Run();

		void Dispatch(CPacket* pPacket);

		bool IsRun() { return m_bRun; }

		void Send_OPEN_SESSION_REQ(std::shared_ptr<CSession> pSession);
		void Send_CLOSE_SESSION_REQ(std::shared_ptr<CSession> pSession);
		void Send_FAIL_CONNECT_REQ(INT8 type);

		void Recv_CLOSE_SESSION_REQ(void* pData);

	protected:
		void _system_send(CPacket* pPacket);

	private:
		volatile bool	m_bRun;
	};

}

