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
	sprintf(msg, "%s join the room", id);
	printf("%s\n", msg);
	send_all_clnt(msg, clnt_sock);		// Send join msg to all clients

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

		// Send Message to Someone (Whispering)
		char whiprefix[] = "whi";
		if (strncmp(msg, whiprefix, strlen(whiprefix)) == 0) {
			char* id_ptr = strtok(msg, " ");
			id_ptr = strtok(NULL, " ");
			// if : check validity - *id_ptr is running?
			char* msg_ptr = strtok(NULL, " ");

			printf("%s -> %s : %s\n", id, id_ptr, msg_ptr);
			char send_msg[300];
			sprintf(send_msg, "\033[0;36m[Send to %s] : %s\033[0m", id_ptr, msg_ptr);
			send_msg[strlen(send_msg)] = '\0';
			send(clnt_sock, send_msg, strlen(send_msg) + 1, 0);			// send ACK to client 
			// send to destination client
			continue;
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