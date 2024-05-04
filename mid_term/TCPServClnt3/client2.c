#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include <ws2tcpip.h> // ����2 Ȯ�� ���

#pragma comment(lib, "ws2_32");


char *SERVERIP = (char *)"127.0.0.1";
int SERVERPORT = 9000;
#define BUFSIZE    512
char hiserver[] = "Hello Server!";

void ErrorHandling(char* message);

int main(int argc, char *argv[])
{
	int retval;
	int sendnum = 0;

	// ����� �μ��� ������ IP �ּҷ� ���
	if (argc > 1) SERVERPORT = atoi(argv[1]);

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) ErrorHandling("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) ErrorHandling("connect()");

	// ���� ���� �� ������ �޽����� ����
	printf("[TCP Client] Server connection: IP=%s Port=%d\n", inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));
	if (send(sock, hiserver, sizeof(hiserver), 0) == SOCKET_ERROR) 
		ErrorHandling("Fail to send Hi message!");

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE + 1];
	int len;

	// �����κ��� �޽����� ����
	retval = recv(sock, buf, BUFSIZE, 0);
	if (retval == SOCKET_ERROR)
		ErrorHandling("Fail to get Hi message!");

	// ���� �޽����� Hi Client���� Ȯ��
	buf[retval] = '\0';
	if (strcmp(buf, "Hi Client!") == 0)
		printf("[TCP/%s:%d] %s\n\n", inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port), buf);
	else ErrorHandling("Got wrong message from Server!");

	// ������ ������ ���
	while (1) {
		// ������ �Է�
		printf("[Message (size <= 20 B, Bye Server!)] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = (int)strlen(buf);
		buf[len - 1] = '\0';
		len -= 1;
		
		// �׳� ���͸� ���� ���
		if (len == 0) {
			printf("You Have To Say At Least One Word!\n\n");
			continue;
		}

		// Bye Server�� �Է��� ���
		if (strcmp(buf, "Bye Server!") == 0)
			send(sock, buf, (int)strlen(buf), 0);

		else {
			// �޽����� 20����Ʈ���� ���� ���
			if (len < 20) {
				for (int i = len; i < 20; i++)
					buf[i] = '#';
			}

			buf[20] = '\0';

			// ������ ������
			retval = send(sock, buf, (int)strlen(buf), 0);
			if (retval == SOCKET_ERROR) {
				ErrorHandling("send()");
				break;
			}
			printf("[TCP Client: to Server port %d] %d B sent (Original : %d B)\n", ntohs(serveraddr.sin_port), retval, len);
			sendnum += 1;
		}

		
		// ������ �ޱ�
		retval = recv(sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			ErrorHandling("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ���� �޽����� Bye Client�� ���
		buf[retval] = '\0';
		if (strcmp(buf, "Bye Client!") == 0) {
			printf("[TCP/%s:%d] %s\n", inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port), buf);
			printf("[TCP Client: from Server port %d] %d messages sent\n\n", ntohs(serveraddr.sin_port), sendnum);
			break;
		}

		// ���� ������ ���
		printf("[TCP Client: from Server port %d] %d B received\n\n", ntohs(serveraddr.sin_port), retval);
	}

	// ���� �ݱ�
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}