#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h> // 윈속2 확장 헤더

#pragma comment(lib, "ws2_32");


int SERVERPORT = 9000;
#define BUFSIZE    512
char hiclient[] = "Hi Client!";
char biclient[] = "Bye Client!";

void ErrorHandling(char* message);

int main(int argc, char *argv[])
{
	int retval;

	// 명령형 인수 확인
	if (argc > 1) SERVERPORT = atoi(argv[1]);

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) ErrorHandling("socket()");

	// bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) ErrorHandling("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) ErrorHandling("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	int totalsize = 0;


	// accept()
	addrlen = sizeof(clientaddr);
	client_sock = accept(listen_sock, (struct sockaddr *)&clientaddr, &addrlen);
	if (client_sock == INVALID_SOCKET) {
		ErrorHandling("accept()");
	}

	// 접속한 클라이언트 정보 출력
	char addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
	printf("\n[TCP Server] Client connection: IP =%s, Port=%d\n",
		addr, ntohs(clientaddr.sin_port));
		
	// 클라이언트로부터 메시지를 수신
	retval = recv(client_sock, buf, BUFSIZE, 0);
	if (retval == SOCKET_ERROR)
		ErrorHandling("Fail to send Hi message!");

	// 받은 메시지가 Hello Server인지 확인
	buf[retval] = '\0';
	if (strcmp(buf, "Hello Server!") == 0) {
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
	}
	else ErrorHandling("Got Wrong message from client!");


	// 정상적으로 메시지를 수신했을 경우 클라이언트에 메시지를 송신
	if (send(client_sock, hiclient, sizeof(hiclient), 0) == SOCKET_ERROR)
		ErrorHandling("Fail to send Hi message!");


	// 클라이언트와 데이터 통신
	while (1) {
		// 데이터 받기
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			ErrorHandling("recv()");
			break;
		}
		else if (retval == 0)
			break;

		buf[retval] = '\0';

		// Bye Server를 수신한 경우
		if (strcmp(buf, "Bye Server!") == 0) {
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);
			printf("[TCP/%s:%d] %d B received\n", addr, ntohs(clientaddr.sin_port), totalsize);
			send(client_sock, biclient, sizeof(biclient), 0);
			break;
		} 

		// 받은 데이터 출력
		printf("[TCP/%s:%d] %s (%d B)\n", addr, ntohs(clientaddr.sin_port), buf, strlen(buf));

		totalsize += strlen(buf);

		// 데이터 보내기
		retval = send(client_sock, buf, (int)strlen(buf), 0);
		if (retval == SOCKET_ERROR) {
			ErrorHandling("send()");
			break;
		}
	}

	// 소켓 닫기
	closesocket(client_sock);
	printf("[TCP Server] Client disconnection: IP=%s, Port=%d\n",
		addr, ntohs(clientaddr.sin_port));
	

	// 소켓 닫기
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}


void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}