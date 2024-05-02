#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <WS2tcpip.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#pragma comment(lib, "ws2_32")

#define BUFFSIZE 1024
void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
	// Arguments
	if (argc != 2) {
		printf("Usage : %s <PORT>\n", argv[0]);
		exit(1);
	}

	// Var Def
	WSADATA wsa;
	SOCKET servsock, clntsock;
	SOCKADDR_IN servaddr, clntaddr;

	int oprnum, recvlen, recvcnt, result, szclntaddr = sizeof(clntaddr);
	char oprmsg[BUFFSIZE];

	// WSAStartup
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) ErrorHandling("WSAStartup() error!");

	// socket
	servsock = socket(PF_INET, SOCK_STREAM, 0);
	if(servsock == INVALID_SOCKET) ErrorHandling("socket() error!");

	// Init servaddr
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	// bind
	if(bind(servsock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) ErrorHandling("bind() error!");

	// listen
	if(listen(servsock, 5) == SOCKET_ERROR) ErrorHandling("listen() error!");

	// accept
	clntsock = accept(servsock, (SOCKADDR*)&clntaddr, &szclntaddr);
	if (clntsock == INVALID_SOCKET) ErrorHandling("socket() error!");
	printf("Connected Client : IP %s, Port %d\n", inet_ntoa(clntaddr.sin_addr), ntohs(clntaddr.sin_port));

	// recv
	oprnum = 0;
	recv(clntsock, &oprnum, 1, 0);
	printf("%d\n", oprnum);

	recvlen = 0;
	while (recvlen < (oprnum * 4 + 1)) {
		recvcnt = recv(clntsock, &oprmsg[recvlen], BUFFSIZE-1, 0);
		recvlen += recvcnt;
	}

	// send
	result = calculate(oprnum, (int*)oprmsg, oprmsg[recvlen - 1]);
	send(clntsock, (char*)&result, sizeof(result), 0);
	
	// close
	closesocket(servsock);
	closesocket(clntsock);
	WSACleanup();

	return 0;
}

void ErrorHandling(char* message)
{
	puts(message, stderr);
	putc('\n', stderr);
	exit(1);
}

int calculate(int num, int numlist[], char oper) {
	int result = numlist[0], i;

	switch (oper) {
	case'+':
		for (i = 1; i < num; i++) result += numlist[i];
		break;
	case'-':
		for (i = 1; i < num; i++) result -= numlist[i];
		break;
	case'*':
		for (i = 1; i < num; i++) result *= numlist[i];
		break;
	}

	return result;
}