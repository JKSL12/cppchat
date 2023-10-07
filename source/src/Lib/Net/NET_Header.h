#pragma once

namespace NET
{
	struct HEADER
	{
		INT32	size = 0;		// 크기
		UINT8	checksum = 0;	// 채크섬
		INT32	type = 0;		// 종류
		INT16	param = 0;
		UINT8	flat = 0;		// 플랫버퍼 사용 여부
	};

	const int HEADER_SIZE = sizeof( HEADER );
}