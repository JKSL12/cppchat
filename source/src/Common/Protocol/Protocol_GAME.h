// Protocol_GAME.h : GAME ���� �������� ����
//

#pragma once

#ifndef _PROTOCOL_GAME_H_
	#define _PROTOCOL_GAME_H_
#endif

#include "NET/PROTOCOL/Protocol_BASE.h"

// ���ӽ����̽� ����
namespace PROTOCOL
{
	namespace ID
	{
		// ��Ŷ ID ����
		enum eGAME_PACKET_ID
		{
			GS_PACKET_BEGIN = PROTOCOL::ID::PROTOCOL_GAME,
			GS_LOGIN_REQ,
			GS_LOGIN_RES,
			GS_KEEP_ALIVE_NFY,

			GS_CONNECTION_PACKET_END, ////////////

			GS_CHAT_REQ,
			GS_CHAT_RES,

			GS_ENTERCHAT_NFY,
			GS_LEAVECHAT_NFY,

			GS_LOGOUT_NFY,

			GS_PACKET_END,
		};
	}

	// ��Ŷ ����
#pragma pack(push, 1)

	struct GS_LOGIN_REQ
	{
		long	account_seq;
	};

	struct GS_LOGIN_RES
	{

	};

	struct GS_KEEP_ALIVE_NFY
	{

	};

	struct GS_CHAT_REQ
	{
		long	account_seq;
		char	szText[512];
	};

	struct GS_CHAT_RES
	{
		long	account_seq;
		char	szText[512];
	};

	struct GS_ENTERCHAT_NFY
	{
		long account_seq;
	};

	struct GS_LEAVECHAT_NFY
	{
		long account_seq;
	};

#pragma pack(pop)

} // namespace PROTOCOL
