#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_PORT 12345
#define TIMEOUT_MS 3000  // 5��

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    char buffer[1024] = { 0 };

    // Winsock �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup ����\n";
        return 1;
    }

    // ���� ����
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "���� ���� ����: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    // ���� �ּ� ����
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    // ���ε�
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "���ε� ����: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // ���� ���
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        std::cerr << "���� ��� ����: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "������ " << SERVER_PORT << " ��Ʈ���� ��� ��...\n";

    // Ŭ���̾�Ʈ ���� ����
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Ŭ���̾�Ʈ ���� ����: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Ŭ���̾�Ʈ ���� ������\n";

    // Ÿ�Ӿƿ� ����
    int timeout = TIMEOUT_MS;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        std::cerr << "Ÿ�Ӿƿ� ���� ����: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // ������ ���� ���
    int n = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (n == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAETIMEDOUT) {
            std::cerr << "Ÿ�Ӿƿ�: ������ ���� ����\n";
        }
        else {
            std::cerr << "������ ���� ����: " << WSAGetLastError() << "\n";
        }
    }
    else {
        buffer[n] = '\0';
        std::cout << "���ŵ� ������: " << buffer << "\n";
    }

    // ���� �ݱ�
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
