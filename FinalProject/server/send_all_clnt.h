// send_all_clnt.h
#ifndef SEND_ALL_CLNT_H
#define SEND_ALL_CLNT_H

////////////////////////////////////////////////////////////////////////////////
// Define & Include
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32")

////////////////////////////////////////////////////////////////////////////////
// Global Var
extern SOCKET g_clnt_socks[];  // Socket List
extern int g_clnt_count;               // Client Count

extern HANDLE hMutex;                  // Socket List Mutex

////////////////////////////////////////////////////////////////////////////////

void send_all_clnt(char* msg, SOCKET my_sock);

#endif // SEND_ALL_CLNT_H
