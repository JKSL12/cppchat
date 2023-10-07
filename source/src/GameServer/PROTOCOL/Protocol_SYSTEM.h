#pragma once

#include "../../Lib/Net/PROTOCOL/Protocol_BASE.h"

namespace PROTOCOL
{
	namespace ID
	{
		enum eSYSTEM_PACKET_ID
		{
			SM_PACKET_BEGIN = PROTOCOL::ID::PROTOCOL_SYSTEM,

			SM_OPEN_SESSION_REQ,
			SM_CLOSE_SESSION_REQ,
			SM_FAIL_CONNECT_REQ,

			SM_SCHEDULE_CMD_REQ,

			SM_PACKET_END

		}; // enum eSYSTEM_PACKET_ID

	}

#pragma pack(push, 1)

	struct SM_OPEN_SESSION_REQ
	{
		INT8	Type;
		INT16	SSNID;
		TCHAR	IP[20];
		UINT16	Port;
	};

	struct SM_CLOSE_SESSION_REQ
	{
		INT16	SSNID;
		INT8	Mode;
	};

	struct SM_FAIL_CONNECT_REQ
	{
		INT8	Type;
	};

	struct SM_SCHEDULE_CMD_REQ
	{
		UINT8	Cmd;
	};

#pragma pack(pop)
}