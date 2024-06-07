// clnt_connection.h
#ifndef CLNT_CONNECTION_H
#define CLNT_CONNECTION_H

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
#include <time.h>
#include "ErrorHandling.h"
#include "recommend_topic.h"

#define BUFSIZE    1000		// Message Buffer Size
#define MAXROOM    10          // Max Room Count
#define ROOMNAME_LEN 30         // Max Room Name Length

////////////////////////////////////////////////////////////////////////////////
// Global Var
extern SOCKET g_clnt_socks[];						// Socket List
extern int g_clnt_count;							// Client Count
extern int g_log_count;								// Client Count

typedef struct connect_list {
	char con_id[30];								// Connect Clients List
	SOCKET con_sock;
	int roomnum;
};

struct connect_list g_connect_list[300];			// Connect Clients List

extern HANDLE hMutex;								// Socket List Mutex

// Room management
char g_room_name[MAXROOM][ROOMNAME_LEN];			// Room names

extern int g_room_count;

DWORD WINAPI clnt_connection(LPVOID arg);

#endif // CLNT_CONNECTION_H