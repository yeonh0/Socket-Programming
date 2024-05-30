// rcv.c
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

DWORD WINAPI rcv(LPVOID arg) {
	// Local Variable
	SOCKET sock = (SOCKET)arg;		// Client Socket
	int str_len = 0;					// Message Length
	char msg[BUFFSIZE];					// Message Buffer

	// While
	while (1) {
		str_len = recv(sock, msg, sizeof(msg), 0);
		if (str_len == -1) exit(1);

		// Remove Enter
		for (int i = 0; msg[i] != 0; i++) {
			if (msg[i] == '\n') {
				msg[i] = '\0';
				break;
			}
		}

		printf("%s\n", msg);
	}

	// Thread Exit
	ExitThread(0);
	return 0;
}