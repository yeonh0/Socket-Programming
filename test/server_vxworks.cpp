#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#define SERVER_PORT 12345
#define TIMEOUT_SEC 5

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    struct timeval timeout;

    // 소켓 생성
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("소켓 생성 실패");
        return -1;
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    // 바인딩
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("바인딩 실패");
        close(server_sock);
        return -1;
    }

    // 연결 대기
    if (listen(server_sock, 5) < 0) {
        perror("연결 대기 실패");
        close(server_sock);
        return -1;
    }

    printf("서버가 %d 포트에서 대기 중...\n", SERVER_PORT);

    // 클라이언트 연결 수락
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
    if (client_sock < 0) {
        perror("클라이언트 연결 실패");
        close(server_sock);
        return -1;
    }

    printf("클라이언트 연결 수락됨\n");

    // 타임아웃 설정 (recv 함수에 대한 타임아웃)
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;

    if (setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("타임아웃 설정 실패");
        close(client_sock);
        close(server_sock);
        return -1;
    }

    // 데이터 수신 대기
    char buffer[1024] = {0};
    int n = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (n < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            printf("타임아웃: 데이터 수신 실패\n");
        } else {
            perror("데이터 수신 오류");
        }
    } else {
        buffer[n] = '\0';
        printf("수신된 데이터: %s\n", buffer);
    }

    // 소켓 닫기
    close(client_sock);
    close(server_sock);

    return 0;
}
