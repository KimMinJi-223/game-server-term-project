#pragma once

#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include "..\..\Server_GameServerTermProject\Server_GameServerTermProject\protocol.h"

#include "Types.h"
#include "Defines.h"
#include "Enums.h"
#include "Values.h"

#include <Windows.h>
#include "Utils.h"
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <Set>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <functional>
#include <assert.h>

// C++20
#include <format>
#include <filesystem>
namespace fs = std::filesystem;

#define _CRTDEBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#pragma comment(lib, "msimg32.lib")

// »ç¿îµå
#include <mmsystem.h>
#include <dsound.h>
#pragma comment(lib, "winmm.lib");
#pragma comment(lib, "dsound.lib");