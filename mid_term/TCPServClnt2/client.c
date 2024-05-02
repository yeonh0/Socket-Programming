#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <WS2tcpip.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#pragma comment(lib, "ws2_32")

#define BUFFSIZE 1024

void ErrorHandling(char *message);

int main(int argc, char *argv[])
{
	// Arguments
	if (argc != 3) {
		printf("Usage : %s <IP> <PORT>", argv[0]);
		exit(1);
	}

	// Var Def
	WSADATA wsa;
	SOCKET hsock;
	SOCKADDR_IN haddr, localaddr;

	int szlocaladdr = sizeof(localaddr), opnum, result;
	char opermsg[BUFFSIZE];

	// WSAstartup
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) ErrorHandling("WSAStartup() error!");

	// socket
	hsock = socket(PF_INET, SOCK_STREAM, 0);
	if(hsock == INVALID_SOCKET) ErrorHandling("socket() error!");

	// Init addr
	memset(&haddr, 0, sizeof(haddr));
	haddr.sin_family = AF_INET;
	haddr.sin_addr.s_addr = inet_addr(argv[1]);
	haddr.sin_port = htons(atoi(argv[2]));

	// connect
	if(connect(hsock, (SOCKADDR*)&haddr, sizeof(haddr)) == SOCKET_ERROR) ErrorHandling("connect() error!");
	getsockname(hsock, (SOCKADDR*)&localaddr, &szlocaladdr);
	printf("Connected! Your IP, Port : %s, %d  || Server IP, Port : %s, %d\n", inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port), inet_ntoa(haddr.sin_addr), ntohs(haddr.sin_port));

	// oper
	printf("Operand count: ");
	scanf("%d", &opnum);
	opermsg[0] = (char)opnum;
	fgetc(stdin);

	for (int i = 0; i < opnum; i++) {
		printf("Operand %d: ", i + 1);
		scanf("%d", (int*)&opermsg[i * 4 + 1]);
		fgetc(stdin);
	}

	printf("Operator: ");
	scanf("%c", &opermsg[opnum * 4 + 1]);

	// send
	send(hsock, opermsg, opnum * 4 + 2, 0);
	
	// recv
	recv(hsock, &result, 4, 0);
	printf("Operation result: %d\n", result);

	// close
	closesocket(hsock);
	WSACleanup();

	return 0;
}

void ErrorHandling(char *message)
{
	puts(message, stderr);
	putc('\n', stderr);
	exit(1);
}