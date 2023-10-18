#include "pch.h"
#include "SERVER/GameServer.h"

void Run();

int main(int argc, wchar_t* argv[])
{
	Run();
}

void Run()
{
	// STEP 0: 게임 서버 객체 선언
	CGameServer gameServer;

	if (gameServer.PreInitialize() == eSERVER_RESULT::SERVER_FAIL)
	{
		printf("PreInitialize() 실행 오류\n");
		return;
	}

	// STEP 7: 서버 프레임워크 초기화
	bool result = gameServer.Initialize(
		GLOBAL::SESSION_MAX,
		GLOBAL::SERVER_SESSION_MAX,
		GLOBAL::IO_THREAD_MAX,
		GLOBAL::DISPATCH_TYPE_MAX,
		GLOBAL::SYSTEM_DISPATCH_MAX
	);

	// STEP 10: 서버 실행
	gameServer.Run(result);

	// STEP 11: 서버 종료 처리
	gameServer.Finalize();
}