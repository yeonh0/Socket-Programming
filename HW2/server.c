#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32");
void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
	WSADATA	wsaData;
	SOCKET hServSock, hClntSock;		
	SOCKADDR_IN servAddr, clntAddr;
	struct in_addr clntaddr;

	int szClntAddr;
	char message[]="Hello";
	char wrongmsg[] = "Wrong Client";
	char sendMsg[1024];
	int sumOfSendMsg = 0;

	if(argc!=3) 
	{
		printf("Usage : %s <port> <client IP>\n", argv[0]);
		exit(1);
	}
	
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!"); 
	
	hServSock=socket(PF_INET, SOCK_STREAM, 0);
	if(hServSock==INVALID_SOCKET)
		ErrorHandling("socket() error");

  
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;

	//servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	// 서버의 주소, 포트를 설정
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port=htons(atoi(argv[1]));

	if(bind(hServSock, (SOCKADDR*) &servAddr, sizeof(servAddr))==SOCKET_ERROR)
		ErrorHandling("bind() error");  
	
	if (listen(hServSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	szClntAddr=sizeof(clntAddr);    	
	hClntSock=accept(hServSock, (SOCKADDR*)&clntAddr,&szClntAddr);
	if(hClntSock==INVALID_SOCKET)
		ErrorHandling("accept() error");  

	// 연결 된 클라이언트의 IP 주소 
	clntaddr = clntAddr.sin_addr;

	// 허용된 주소인지 확인하기
	// 서버로부터 입력 받은 주소 : inet_addr(argv[2])
	// 접속한 클라이언트 주소 : clntaddr.S_un.S_addr
	if (clntaddr.S_un.S_addr != inet_addr(argv[2])) {
		// 잘못된 IP 주소인 경우 : 서버에 접속한 IP 주소를 출력해주고 연결을 종료한다
		printf("Wrong Client : %u.%u.%u.%u\n", clntaddr.S_un.S_un_b.s_b1, clntaddr.S_un.S_un_b.s_b2, clntaddr.S_un.S_un_b.s_b3, clntaddr.S_un.S_un_b.s_b4);
		send(hClntSock, wrongmsg, sizeof(wrongmsg), 0);
		closesocket(hClntSock);
		closesocket(hServSock);
		WSACleanup();
		return 0;
	}

	int sendLen = send(hClntSock, message, sizeof(message), 0);
	if (sendLen == SOCKET_ERROR)
		ErrorHandling("send() error!");

	while (1) {
		// 클라이언트로부터 수신
		char recvMsg[1024];
		int recvLen = recv(hClntSock, recvMsg, sizeof(recvMsg), 0);

		if (recvLen <= 0) {
			ErrorHandling("recv() error or connection closed");
		}

		// END 수신 경우
		if (strcmp(recvMsg, "END") == 0) {
			printf("END : Disconnected. Close Socket.\n");			
			printf("Recv Msg Sum : %d\n", sumOfSendMsg);
			closesocket(hClntSock);
			closesocket(hServSock);
			WSACleanup();
			return 0;
		}

		recvMsg[recvLen] = '\0'; // 문자열 종료를 위해 NULL 문자 추가
		
		// 수신한 메시지 출력, 메시지 길이 sum
		printf("Received message from client: %s\n", recvMsg);
		sumOfSendMsg = sumOfSendMsg + recvLen;
		
		// 수신한 메시지 길이 전송
		char strRecvLen[20];
		sprintf(strRecvLen, "%d", recvLen);
		send(hClntSock, strRecvLen, sizeof(strRecvLen), 0);
	}
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}