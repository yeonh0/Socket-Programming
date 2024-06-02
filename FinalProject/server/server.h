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
#include <time.h>
#include "send_all_clnt.h"
#include "ErrorHandling.h"
#include "clnt_connection.h"
#include "SIGNIN.h"
#include "SIGNUP.h"

#pragma comment(lib, "ws2_32")

////////////////////////////////////////////////////////////////////////////////
// User Define
#define CLNT_MAX   1000			// Max Client
#define SERVERPORT 3500			// Server Socket Port

////////////////////////////////////////////////////////////////////////////////
// Global Variables
SOCKET g_clnt_socks[CLNT_MAX];  // Socket List
int g_clnt_count = 0;           // Client Count

HANDLE hMutex;					// Socket List Mutex

typedef struct {
	char con_id[30];								// Connect Clients List
	SOCKET con_sock;
} connect_list;

struct connect_list g_connect_list[300];			// Connect Clients List

int g_log_count = 0;			// Login Client Count

#endif // SERVER_H