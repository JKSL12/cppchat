#pragma once

namespace NET
{
	class CPacket;

	class CSendQueue
	{
	public:
		CSendQueue();
		~CSendQueue();

		void	Push(INT16 SSNID, CPacket* pPacket);
	};
}