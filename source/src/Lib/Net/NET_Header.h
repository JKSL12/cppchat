#pragma once

namespace NET
{
	struct HEADER
	{
		INT32	size = 0;		// ũ��
		UINT8	checksum = 0;	// äũ��
		INT32	type = 0;		// ����
		INT16	param = 0;
		UINT8	flat = 0;		// �÷����� ��� ����
	};

	const int HEADER_SIZE = sizeof( HEADER );
}