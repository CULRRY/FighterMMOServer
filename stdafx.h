#pragma once

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "winmm.lib")

#include <iostream>

#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <timeapi.h>

using namespace std;

#include "Types.h"
#include "Macro.h"