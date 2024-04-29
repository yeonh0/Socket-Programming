#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

#define MAX_MSG_LENGTH 20

void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	char recvMsg[30];
	int strLen;
	char wrongmsg[] = "Wrong Client";

	if(argc!=3)
	{
		printf("Usage : %s <server IP> <server port>\n", argv[0]);
		exit(1);
	}
	
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");  
	
	hSocket=socket(PF_INET, SOCK_STREAM, 0);
	if(hSocket==INVALID_SOCKET)
		ErrorHandling("hSocketet() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=inet_addr(argv[1]);
	servAddr.sin_port=htons(atoi(argv[2]));
	

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr))==SOCKET_ERROR)
		ErrorHandling("connect() error!");
	
	strLen = recv(hSocket, recvMsg, sizeof(recvMsg) - 1, 0);

	if (strcmp(wrongmsg, recvMsg) == 0) {
		printf("From server : %s \n", recvMsg);
		closesocket(hSocket);
		WSACleanup();
		return 0;
	}
	
	printf("CONNECT SUCCESS!!\n");


	while (1) {
		// 클라이언트에서 입력 받음
		char inputMsg[1024];
		char* sendMsg;
		
		printf("Enter message to send to server: ");
		if (fgets(inputMsg, sizeof(inputMsg), stdin) == NULL) {
			ErrorHandling("fgets() error");
			return 1;
		}

		// Null 제거
		inputMsg[strcspn(inputMsg, "\n")] = '\0';

		// 입력받은 메시지가 end인 경우, 소켓을 닫고 종료한다
		if (strcmp(inputMsg, "END") == 0) {
			send(hSocket, inputMsg, MAX_MSG_LENGTH, 0);
			printf("END : Disconnected. Close Socket.\n");
			closesocket(hSocket);
			WSACleanup();
			return 0;
		}

		// 입력받은 메시지를 전송한다
		size_t msgLen = strlen(inputMsg);


		// 메시지 길이가 20을 넘지 않는 경우
		if (msgLen < MAX_MSG_LENGTH) {
			for (size_t i = msgLen; i < MAX_MSG_LENGTH; ++i) {
				inputMsg[i] = '*';
			}
		}


		int sendLen = send(hSocket, inputMsg, MAX_MSG_LENGTH, 0);
		//int sendLen = send(hSocket, inputMsg, msgLen, 0);
		if (sendLen == SOCKET_ERROR)
			ErrorHandling("send() error!");


		// 서버로부터 수신한 메시지
		strLen = recv(hSocket, recvMsg, sizeof(recvMsg) - 1, 0);
		printf("\nFrom server : %s \n", recvMsg);

		if (strLen == -1)
			ErrorHandling("read() error!");

		recvMsg[strLen] = '\0';
	}

	  
	
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}