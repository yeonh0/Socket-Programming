#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include<stdio.h>
#pragma comment(lib, "ws2_32")

void ErrorHandling(char *message);

int main(int argc, char *argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	u_short port_h = 0x1234;
	u_long ipaddr_h = 0x12345678;
	u_short port_n;
	u_long ipaddr_n;

	printf("[Host Byte -> Network Byte]\n");
	printf("%#x -> %#x\n", port_h, port_n = htons(port_h));
	printf("%#x -> %#x\n", ipaddr_h, ipaddr_n = htonl(ipaddr_h));

	printf("[Network Byte -> Host Byte]\n");
	printf("%#x -> %#x\n", port_n, ntohs(port_n));
	printf("%#x -> %#x\n", ipaddr_n, ntohl(ipaddr_n));


	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}