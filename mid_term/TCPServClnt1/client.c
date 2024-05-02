#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#pragma comment(lib, "ws2_32")

void ErrorHandling(char *message);

int main(int argc, char *argv[])
{
	WSADATA wsa;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	char message[30];
	int strLen;

	if (argc != 3) {
		printf("Usage : %s <IP> <Port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) ErrorHandling("WSA Startup Error!");

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET) ErrorHandling("hSocket() Error!");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) ErrorHandling("Connect() Error!");

	while(1) {
		strLen = recv(hSocket, message, sizeof(message) - 1, 0);
		if (strLen == -1) ErrorHandling("Recv() Error !");

		if (strcmp(message, "END") == 0) {
			closesocket(hSocket);
			WSACleanup();
			return 1;
		}
		printf("Message From Server : %s\n", message);
	}

	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}