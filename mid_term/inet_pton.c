#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include<stdio.h>
#pragma comment(lib, "ws2_32")

void ErrorHandling(char *message);

int main(int argc, char *argv[]) {
    struct in_addr ipv4num;
    const char* ipstr = "192.168.1.3";
    char ipv4str[INET_ADDRSTRLEN];

    printf("IP : %s\n", ipstr);
    inet_pton(AF_INET, ipstr, &ipv4num);
    printf("IP(net) : %#x\n\n", ipv4num.s_addr);

    inet_ntop(AF_INET, &ipv4num, ipv4str, sizeof(ipv4str));

    printf("IP : %s\n", ipv4str);
    
    return 0;
}

void ErrorHandling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}