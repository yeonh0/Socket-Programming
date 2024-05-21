////////////////////////////////////////////////////////////////////////////////
// Header File
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32");

////////////////////////////////////////////////////////////////////////////////
// User Define
#define BUFSIZE    1000		// Message Buffer Size
#define CLNT_MAX   1000		// Max Client
#define SERVERPORT 3500		// Server Socket Port

////////////////////////////////////////////////////////////////////////////////
// Function
DWORD WINAPI clnt_connection(LPVOID arg);
void send_all_clnt(char* msg, SOCKET my_sock);
void ErrorHandling(char* message);

////////////////////////////////////////////////////////////////////////////////
// Global Variables
SOCKET g_clnt_socks[CLNT_MAX];  // Socket List
int g_clnt_count = 0;           // Client Count

HANDLE hMutex;					// Socket List Mutex

////////////////////////////////////////////////////////////////////////////////
// Main Function
int main(int argc, char* argv[])
{
	// Local Variable
	WSADATA wsa;								// WSA DATA
	SOCKET serv_sock, clnt_sock;				// Create Sockets
	struct sockaddr_in servaddr, clntaddr;		// Cread SockAddrs
	int clnt_addr_size = sizeof(clntaddr);		// Size of Client Addr
	HANDLE t_thread;							// Create Thread
	int retval;									// Return Value


	// INITITIALIZE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Mutex
	hMutex = CreateMutexW(NULL, FALSE, NULL);
	if (hMutex == NULL) ErrorHandling("ERROR: CreateMutex() error!");

	// Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// Server Socket - Server IP : ANY, Port : 3500
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock == INVALID_SOCKET) ErrorHandling("ERROR: Server Socket INIT error!");

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVERPORT);


	// Binding !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	retval = bind(serv_sock, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (retval == SOCKET_ERROR) ErrorHandling("ERROR: Server Socket Binding error!");


	// Listening !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	retval = listen(serv_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) ErrorHandling("listen()");


	// While  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	while (1) {
		// Accept & Return Client Socket
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clntaddr, &clnt_addr_size);
		if (clnt_sock == INVALID_SOCKET) ErrorHandling("ERROR: Client Socket Accepting error!");

		// Print Information of the Clent
		printf("Client Connect: %s:%d\n", inet_ntoa(clntaddr.sin_addr), ntohs(clntaddr.sin_port));

		// Add Client Socket to Client Socket List
		WaitForSingleObject(hMutex, INFINITE);
		g_clnt_socks[g_clnt_count++] = clnt_sock;
		ReleaseMutex(hMutex);

		// Create Thread : clnt_connection((void*)clnt_sock)
		t_thread = CreateThread(NULL, 0, clnt_connection, (LPVOID)clnt_sock, 0, NULL);
		if (t_thread == -1) ErrorHandling("ERROR: Create Thread error!");
	}

	closesocket(serv_sock);
	CloseHandle(hMutex);
	WSACleanup();

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Thread Function
DWORD WINAPI clnt_connection(LPVOID arg) {
	// Local Variable
	SOCKET clnt_sock = (SOCKET)arg;		// Client Socket
	int str_len = 0;					// Message Length
	char msg[BUFSIZE];					// Message Buffer
	int id[100];						// Client ID

	// Get Client ID
	recv(clnt_sock, id, sizeof(id), 0);
	printf("%s join the room\n", id);

	// while : Keep Reading Message
	while (1) {
		// Read Message from the Client
		str_len = recv(clnt_sock, msg, sizeof(msg), 0);
		if (str_len == SOCKET_ERROR) {
			ErrorHandling("Reading error");
		}

		// Remove Enter
		for (int i = 0; msg[i] != 0; i++) {
			if (msg[i] == '\n') {
				msg[i] = '\0';
				break;
			}
		}

		// break
		if (strcmp(msg, "quit") == 0) {
			sprintf(msg, "%s has left the room", id);
			printf("%s\n", msg);
			send_all_clnt(msg, clnt_sock);
			break;
		}

		// Send Message to the ALL Clients & Print Message on the Server Program
		send_all_clnt(msg, clnt_sock);
		printf("%s\n", msg);
	}

	// Remove the client socket from the socket list
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < g_clnt_count; i++) {
		if (clnt_sock == g_clnt_socks[i]) {
			for (; i < g_clnt_count - 1; i++)
				g_clnt_socks[i] = g_clnt_socks[i + 1];
			break;
		}
	}
	g_clnt_count--;
	ReleaseMutex(hMutex);

	// Close the Client Socket
	closesocket(clnt_sock);

	// Exit Thread
	ExitThread(0);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Send Message to the ALL Clients
void send_all_clnt(char* msg, SOCKET my_sock) {
	// Sending
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < g_clnt_count; i++) {
		if (my_sock != g_clnt_socks[i]) {
			printf("Send Message : %s\n", msg);
			send(g_clnt_socks[i], msg, strlen(msg) + 1, 0);
		}
	}
	ReleaseMutex(hMutex);
}

////////////////////////////////////////////////////////////////////////////////
// Error Handling Function
void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}