#pragma once

namespace PROTOCOL
{
	namespace ID
	{
		enum eBASE_ID
		{
			PROTOCOL_SYSTEM = 0x00000000,
			PROTOCOL_GAME = 0x01000000,			
		};
	};

	inline INT32 GET_PROTOCOL_ID(INT32 type)
	{
		return ((type) & (0xFF000000));
	}
}