// send_all_clnt.c
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
// Send Message to the ALL Clients
void send_all_clnt(char* msg, SOCKET my_sock) {
	// Sending
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < g_clnt_count; i++) {
		if (my_sock != g_clnt_socks[i]) {
			printf("Send Message : %s\n", msg);
			send(g_clnt_socks[i], msg, strlen(msg) + 1, 0);
		}
	}
	ReleaseMutex(hMutex);
}
