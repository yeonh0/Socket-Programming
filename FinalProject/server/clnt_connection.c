// clnt_connection.c
#include "clnt_connection.h"

DWORD WINAPI clnt_connection(LPVOID arg) {
	// Local Variable
	SOCKET clnt_sock = (SOCKET)arg;		// Client Socket
	int str_len = 0;					// Message Length
	char msg[BUFSIZE];					// Message Buffer
	char id[100], pw[100];				// Client ID, PW
	strcpy(id, "test");	//test

	char* prefix_ptr;
	char* id_ptr = NULL;				// Whisper Dest, Msg
	char* msg_ptr = NULL;

	char whisper_msg[300];
	char clientinfo[30];


	/*
	// Get Client ID
	recv(clnt_sock, id, sizeof(id), 0);
	sprintf(msg, "%s join the room", id);
	printf("%s\n", msg);
	send_all_clnt(msg, clnt_sock);		// Send join msg to all clients*/

	/*
	FILE* fp = fopen("info.txt", "w");
	sprintf(clientinfo, "%s:%d", inet_ntoa(clntaddr.sin_addr), ntohs(clntaddr.sin_port));
	printf("%s\n", clientinfo);
	fputs(clientinfo, fp); 
	fputc('\n', fp);      
	fclose(fp);
	*/

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

		// prefix parsing
		prefix_ptr = strtok(msg, " ");

		// Send Message to the ALL Clients
		if (strcmp(prefix_ptr, "msg") == 0) {
			send_all_clnt(msg, clnt_sock);
			printf("%s\n", msg+4);
		}

		// Send Message to the specific Client
		else if (strcmp(prefix_ptr, "whi") == 0) {
			prefix_ptr = strtok(NULL, " ");
			id_ptr = prefix_ptr;
			msg_ptr = id_ptr + strlen(id_ptr) + 1;

			printf("%s -> %s : %s\n", id, id_ptr, msg_ptr);
			sprintf(whisper_msg, "\033[0;36m[Send to %s] : %s\033[0m", id_ptr, msg_ptr);
			whisper_msg[strlen(whisper_msg)] = '\0';
			send(clnt_sock, whisper_msg, strlen(whisper_msg) + 1, 0);			// send ACK to client 
			// send to destination client
		}
	}

	// Remove the client socket from the socket list
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < g_clnt_count; i++) {
		printf("left clients: %d\n", g_clnt_count);
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