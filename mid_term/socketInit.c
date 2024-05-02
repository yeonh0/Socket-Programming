#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include<stdio.h>
#pragma comment(lib, "ws2_32")

void ErrorHandling(char *message);

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage : %s <IP> <PORT>\n", argv[0]);
        exit(1);
    }

    SOCKADDR_IN addr_host;
    char* serv_ip = "242.123.53.76";
    char* serv_port = "123";
    
    if (memset(&addr_host, 0, sizeof(addr_host)) != 0) ErrorHandling("memset() Error!");
    addr_host.sin_family = PF_INET;
    addr_host.sin_addr.s_addr = inet_addr(serv_ip);
    addr_host.sin_port = htons(atoi(serv_port));

    addr_host.sin_addr.s_addr = ftonl(INADDR_ANY);
    
}

void ErrorHandling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}