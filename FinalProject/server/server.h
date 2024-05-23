// server.h
#ifndef SERVER_H
#define SERVER_H

////////////////////////////////////////////////////////////////////////////////
// Header File
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include "send_all_clnt.h"
#include "ErrorHandling.h"
#include "clnt_connection.h"

#pragma comment(lib, "ws2_32")

////////////////////////////////////////////////////////////////////////////////
// User Define
#define CLNT_MAX   1000		// Max Client
#define SERVERPORT 3500		// Server Socket Port

////////////////////////////////////////////////////////////////////////////////
// Global Variables
SOCKET g_clnt_socks[CLNT_MAX];  // Socket List
int g_clnt_count = 0;           // Client Count

HANDLE hMutex;					// Socket List Mutex

#endif // SERVER_H