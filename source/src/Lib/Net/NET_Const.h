#pragma once

namespace NET
{
	enum eSESSION_TYPE : UINT8
	{
		SESSION_TYPE_MIN = 0,

		USER_SESSION = 0,								// User Session Type
		DBA_SESSION,									// DBA Session Type
		SERVER_SESSION,									// Server Session Type

		SESSION_TYPE_MAX

	};

	const INT16		INVALID_SSNID = -1;			// Invalid Session ID

	const INT16		SYSTEM_QUEUE = 0;			// System Queue Index (RecvQue Index)
	const INT16		USER_QUEUE = 1;			// User Queue Index (RecvQue Index)
	const INT16		DEFAULT_QUEUE_COUNT = 2;			// Default Queue Count (System + User)
	const INT16		QUEUE_COUNT_MAX = 256;			// Queue Count Max

	const UINT16	SESSION_MAX = 3000;			// Client Session Max
	const UINT16	SERVER_SESSION_MAX = 100;			// Server Session Max

	const INT32		PACKET_BODY_SIZE = 1024 * 768;
	const INT32		PACKET_SIZE_MAX = HEADER_SIZE + PACKET_BODY_SIZE;

	const UINT16	WAIT_ACCEPT_MAX = 3;			// Wait Accept Maximum

	//--------------------------------------------------------------------------

	const UINT8		IO_FLAG_NONE = 0x00;
	const UINT8		IO_FLAG_RECV = 0x01;
	const UINT8		IO_FLAG_SEND = 0x02;

}