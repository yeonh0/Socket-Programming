////////////////////////////////////////////////////////////////////////////////
// Header File
#include "server.h"

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
