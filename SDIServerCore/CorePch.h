#pragma once

#include "Types.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "CoreConfig.h"
#include "Container.h"

#include <windows.h>
#include <iostream>
using namespace std;

#include <WinSock2.h>
#include <mswsock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "Lock.h"
#include "ObjectPool.h"
#include "TypeCast.h"
#include "Memory.h"
#include "SendBuffer.h"
#include "Session.h"
#include "Service.h"
#include "LockQueue.h"
#include "Logger.h"

#include "Utils.h"

#include "ServerStats.h"