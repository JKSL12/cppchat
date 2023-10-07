#pragma once

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#define __GAME_SERVER__
#define _ENABLE_ATOMIC_ALIGNMENT_FIX

//------------------------------------------------------------------------------
// 표준 & Windows 헤더 파일
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

#define NOMINMAX

#include <windows.h>
#include <io.h>
#include <atlcoll.h>

#include <stdio.h>
#include <tchar.h>

#include <sstream>
#include <string>
#include <memory>
#include <regex>
#include <mutex>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <list>
#include <forward_list>
#include <deque>
#include <array>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <functional>
#include <algorithm>
#include <chrono>
#include <atomic>
#include <random>

#include "COMMON/SERVER_Types.h"

#include "Net/NET_Pch.h"

#include "Common/Protocol/Protocol_GAME.h"

#include "SERVER/GameGlobal.h"

#include "WORKER/GameWorker.h"