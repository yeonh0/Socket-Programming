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
#include "ErrorHandling.h"
#include "rcv.h"

#pragma comment(lib, "ws2_32")

////////////////////////////////////////////////////////////////////////////////
// User Define
#define BUFSIZE    100		// Buffer Size
#define NAMESIZE   20		// Max Client

////////////////////////////////////////////////////////////////////////////////
// Main Function
int main(int argc, char* argv[])
{
	// Local Variable
	WSADATA wsa;								// WSA DATA
	SOCKET sock;								// Create Server Socket
	struct sockaddr_in servaddr;				// Cread Server SockAddr
	HANDLE t_thread;							// Create Thread
	int retval;									// Return Value

	char chat[BUFSIZE];							// Create Chat Buffer
	char msg[1000];								// Create Sending Message Buffer

	// Check Arguments
	if (argc < 4) {
		printf("Usage : %s <IP> <PORT> <USER ID>\n", argv[0]);
		exit(1);
	}

	// INITITIALIZE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// Server Socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) ErrorHandling("ERROR: Server Socket INIT error!");

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));


	// Connecting !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	retval = connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (retval == SOCKET_ERROR) ErrorHandling("ERROR: Server Socket Connect error!");


	// Send ID to Server !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	sprintf(msg, "%s", argv[3]);
	send(sock, msg, strlen(msg) + 1, 0);

	// Create Thread !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	t_thread = CreateThread(NULL, 0, rcv, (LPVOID)sock, 0, NULL);
	if (t_thread == NULL) ErrorHandling("ERROR: Create Thread error!");


	// While  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	while (1) {
		// Get Message
		fgets(chat, sizeof(chat), stdin);

		// Remove Enter
		for (int i = 0; chat[i] != 0; i++) {
			if (chat[i] == '\n') {
				chat[i] = '\0';
				break;
			}
		}

		// quit
		if (strcmp(chat, "quit") == 0) {
			sprintf(msg, "\033[0;35m%s has left the room\033[0m\n", argv[3]);
			printf("%s", msg);
			send(sock, chat, strlen(chat) + 1, 0);
			break;
		}

		// Send
		sprintf(msg, "[%s] : %s\n", argv[3], chat);
		printf("\033[0;32m%s\033[0m", msg);
		send(sock, msg, strlen(msg) + 1, 0);
	}
	// close socket
	closesocket(sock);
	WSACleanup();
	return 0;
}
