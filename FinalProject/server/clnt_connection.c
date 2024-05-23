// clnt_connection.c
#include "clnt_connection.h"

DWORD WINAPI clnt_connection(LPVOID arg) {
	// Local Variable
	SOCKET clnt_sock = (SOCKET)arg;		// Client Socket
	int str_len = 0;					// Message Length
	char msg[BUFSIZE];					// Message Buffer
	char id[100];						// Client ID

	// Get Client ID
	recv(clnt_sock, id, sizeof(id), 0);
	printf("%s join the room\n", id);

	// while : Keep Reading Message
	while (1) {
		// Read Message from the Client
		str_len = recv(clnt_sock, msg, sizeof(msg), 0);

		// Remove Enter
		for (int i = 0; msg[i] != 0; i++) {
			if (msg[i] == '\n') {
				msg[i] = '\0';
				break;
			}
		}

		// break
		if (strcmp(msg, "quit") == 0 || str_len == SOCKET_ERROR) {
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