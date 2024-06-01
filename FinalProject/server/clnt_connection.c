// clnt_connection.c
#include "clnt_connection.h"

int checkID(char* id) {
	int retval;
	int buf[2000];				// UserInfo Data (ID, PW set)
	char* tok_ptr = NULL;
	char* id_ptr = NULL;

	// ID has Space or Too long - return 1
	if (strchr(id, ' ') != NULL || strlen(id) > 15)
		return 1;

	// Open UserInfo File
	FILE* fp = fopen("info.txt", "r");
	retval = fread(buf, sizeof(char), sizeof(buf), fp);
	fclose(fp);
	buf[retval] = '\0';

	// Enter Tokenizer
	tok_ptr = strtok(buf, "\n");
	while (tok_ptr != NULL) {
		retval = strlen(tok_ptr);
		id_ptr = strtok(tok_ptr, " ");
		tok_ptr = strtok(tok_ptr + retval + 1, "\n");

		if (strcmp(id_ptr, id) == 0)
			// Duplicated ID - return 2
			return 2;
	}

	return 0;
}

DWORD WINAPI clnt_connection(LPVOID arg) {
	// Local Variable
	SOCKET clnt_sock = (SOCKET)arg;		// Client Socket
	int str_len = 0;					// Message Length
	char msg[BUFSIZE];					// Message Buffer
	char id[100], pw[100];				// Client ID, PW
	int retval;
	FILE* fp;

	char* prefix_ptr;
	char* id_ptr = NULL;				// Whisper Dest, Msg, idchk
	char* pw_ptr = NULL;
	char* msg_ptr = NULL;

	char* tok_ptr = NULL;

	char whisper_msg[300];
	char clientinfo[30];

	// while : Keep Reading Message
	while (1) {
		// 1 : sign up, 2 : sign in
		sprintf(msg, "\033[0;36m[SYSTEM]Sign Up : 1, Sign In : 2\033[0m");
		send(clnt_sock, msg, strlen(msg) + 1, 0);
		str_len = recv(clnt_sock, msg, sizeof(msg), 0);

		// Read Message from the Client
		if (str_len == SOCKET_ERROR || strcmp(msg, "quit") == 0)
			break;
		
		// 1-1 : sign up
		if (strcmp(msg, "1") == 0) {

			// Get ID - Read Message from the Client
			sprintf(msg, "\033[0;36m[SYSTEM]Sign Up - Enter ID\033[0m");
			send(clnt_sock, msg, strlen(msg) + 1, 0);

			// Check ID
			if (recv(clnt_sock, id, sizeof(id), 0) == SOCKET_ERROR || strcmp(id, "quit") == 0)
				break;
			retval = checkID(id);

			// Wrong ID 
			if (retval == 1) {
				sprintf(msg, "\033[0;36m[SYSTEM]Wrong ID : No Space, 1~15\033[0m");
				send(clnt_sock, msg, strlen(msg) + 1, 0);
				continue;
			}

			// Duple ID
			if (retval == 2) {
				sprintf(msg, "\033[0;36m[SYSTEM]Wrong ID : Duplicated\033[0m");
				send(clnt_sock, msg, strlen(msg) + 1, 0);
				continue;
			}
			
			// Get PW - Read Message from the Client
			sprintf(msg, "\033[0;36m[SYSTEM]Sign Up - Enter PW\033[0m");
			send(clnt_sock, msg, strlen(msg) + 1, 0);
			if (recv(clnt_sock, pw, sizeof(pw), 0) == SOCKET_ERROR || strcmp(pw, "quit") == 0)
				break;

			// PW has Space or Too long - return 1
			if (strchr(pw, ' ') != NULL || strlen(pw) > 15) {
				sprintf(msg, "\033[0;36m[SYSTEM]Wrong PW : No Space, 1~15\033[0m");
				send(clnt_sock, msg, strlen(msg) + 1, 0);
				continue;
			}

			// Success
			sprintf(msg, "\033[0;36m[SYSTEM]Successfully Signed Up!\033[0m");
			send(clnt_sock, msg, strlen(msg) + 1, 0);
			printf("Signed Up - ID: %s\n", id);

			// Open UserInfo File And Write ID, PW
			if ((fp = fopen("info.txt", "a")) == NULL) {
				printf("File Open Error!\n");
				break;
			}
			if (fprintf(fp, "%s %s\n", id, pw) < 0) {
				printf("Failed to write to file");
				fclose(fp);
				return 1;
			}
			fclose(fp);
		}
			
		// 1-2 : sign in
		if (strcmp(msg, "1") == 0) {
			sprintf(msg, "\033[0;36m[SYSTEM]Sign In - Enter ID\033[0m");
			send(clnt_sock, msg, strlen(msg) + 1, 0);

			// Get ID
			if (recv(clnt_sock, id, sizeof(id), 0) == SOCKET_ERROR || strcmp(id, "quit") == 0)
				break;
		}
		/*

		// prefix parsing
		prefix_ptr = strtok(msg, " ");

		// Send Message to the ALL Clients
		if (strcmp(prefix_ptr, "msg") == 0) {
			send_all_clnt(msg+4, clnt_sock);
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

		// Check ID Duplication
		else if (strcmp(prefix_ptr, "idchk") == 0) {
			strcpy(id, msg+6);
			// Duplication func
			FILE* fp = fopen("info.txt", "r");
			retval = fread(msg, sizeof(char), sizeof(msg), fp);
			fclose(fp);
			msg[retval - 1] = '\0';
			msg[retval] = '\0';

			// Enter Tokenizer
			tok_ptr = strtok(msg, "\n");
			int chkflag = 0;
			while (tok_ptr != NULL) {
				str_len = strlen(tok_ptr);
				id_ptr = strtok(tok_ptr, " ");
				tok_ptr = strtok(tok_ptr + str_len +1, "\n");
				if (strcmp(id_ptr, id) == 0) {
					// Send Message - ID Duplicated
					sprintf(msg, "\033[0;36m[SYSTEM] Duplicated ID - %s!\033[0m", id);
					send(clnt_sock, msg, strlen(msg) + 1, 0);
					sprintf(msg, "\033[0;36m[SYSTEM] Sign In : 1, Sign Up : 2\033[0m");
					send(clnt_sock, msg, strlen(msg) + 1, 0);
					chkflag = 1;
					break;
				}
			}

			if (!chkflag) {
				printf("Successfully Registred!\n");
			}
		}
		*/
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



/*
// Get Client ID
recv(clnt_sock, id, sizeof(id), 0);
sprintf(msg, "%s join the room", id);
printf("%s\n", msg);
send_all_clnt(msg, clnt_sock);		// Send join msg to all clients*/

/*
FILE* fp = fopen("info.txt", "a");
sprintf(clientinfo, "%s:%d", inet_ntoa(clntaddr.sin_addr), ntohs(clntaddr.sin_port));
printf("%s\n", clientinfo);
fputs(clientinfo, fp);
fputc('\n', fp);
fclose(fp);
*/

/*
		// break
		if (strcmp(msg, "quit") == 0 || str_len == SOCKET_ERROR) {
			sprintf(msg, "%s has left the room", id);
			printf("%s\n", msg);
			send_all_clnt(msg, clnt_sock);
			break;
		}
*/