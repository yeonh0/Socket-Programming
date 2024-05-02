#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#pragma comment(lib, "ws2_32")

#define BUF_SIZE 30
void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET sSocket, cSocket;
	SOCKADDR_IN serv_addr, client_addr;

	char message[30];
	int szClntAddr;
	size_t msgLen;

	if (argc != 2) {
		printf("Usage : %s <Port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) ErrorHandling("WSAStartup() Error!");

	sSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (sSocket == INVALID_SOCKET) ErrorHandling("socket() Error!");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(sSocket, (SOCKADDR*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) ErrorHandling("bind() Error!");

	if (listen(sSocket, 5) == SOCKET_ERROR) ErrorHandling("listen() Error!");

	szClntAddr = sizeof(client_addr);
	cSocket = accept(sSocket, (SOCKADDR*)&serv_addr, &szClntAddr);
	if(cSocket == INVALID_SOCKET) ErrorHandling("accept() Error!");

	while (1) {
		printf("Enter Message to send to server : ");
		if (fgets(message, sizeof(message), stdin) == NULL) {
			ErrorHandling("fgets() error!");
			return 1;
		}

		msgLen = strlen(message);
		if (msgLen > 0 & message[msgLen - 1] == '\n') message[msgLen - 1] = '\0';
		printf("YOurmesage : %s\n", message);

		if (strcmp(message, "END") == 0) {
			send(cSocket, message, msgLen, 0);
			closesocket(sSocket);
			closesocket(cSocket);

			WSACleanup();
			return 0;
		}
		send(cSocket, message, sizeof(message)-1, 0);
	}
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}