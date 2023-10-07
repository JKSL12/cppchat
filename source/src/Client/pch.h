#pragma once

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#define __GAME_SERVER__
#define _ENABLE_ATOMIC_ALIGNMENT_FIX

//------------------------------------------------------------------------------
// ǥ�� & Windows ��� ����
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#include "framework.h"

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

#include "Net/NET_Pch.h"

#include "curl_define.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "Common/Protocol/Protocol_GAME.h"