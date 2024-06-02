// rcv.h
#ifndef RCV_H
#define RCV_H

////////////////////////////////////////////////////////////////////////////////
// Define & Header File
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include "ErrorHandling.h"

#define BUFFSIZE    100		// Buffer Size

#pragma comment(lib, "ws2_32")

DWORD WINAPI rcv(LPVOID arg);

#endif // RCV_H