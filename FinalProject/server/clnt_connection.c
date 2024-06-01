// clnt_connection.c
#include "clnt_connection.h"
#include <time.h>

int SIGNUP(char* id) {
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

int SIGNIN(char* id, char* pw) {
	int retval;
	int signinbuf[2000];				// UserInfo Data (ID, PW set)
	char* tok_ptr = NULL;
	char* id_ptr = NULL;
	char* pw_ptr = NULL;

	// ID or PW has Space or Too long - return 1
	if (strchr(id, ' ') != NULL || strlen(id) > 15 || strchr(pw, ' ') != NULL || strlen(pw) > 15)
		return 1;

	// Open UserInfo File
	FILE* file = fopen("info.txt", "r");
	if (file == NULL) {
		fprintf(stderr, "Could not open info.txt for reading\n");
		return 1;
	}

	char line[256];

	while (fgets(line, sizeof(line), file)) {
		// Remove newline character if present
		line[strcspn(line, "\n")] = 0;

		// Split the line into id and pw based on the first space
		char* id_ptr = strtok(line, " ");
		char* pw_ptr = strtok(NULL, " ");

		if (id_ptr && pw_ptr) {
			if ((strcmp(id, id_ptr) == 0) && (strcmp(pw, pw_ptr) == 0)) {
				return 2;
			}
		}
	}

	fclose(file);
	return 1;
}

DWORD WINAPI clnt_connection(LPVOID arg) {
	// Local Variable
	SOCKET clnt_sock = (SOCKET)arg;		// Client Socket
	int str_len = 0;					// Message Length
	char msg[BUFSIZE];					// Message Buffer
	char chat[BUFSIZE];
	char id[100], pw[100];				// Client ID, PW
	int retval;
	
	FILE* fp;
	char* id_ptr = NULL;				// Whisper Dest, Msg, idchk
	char* pw_ptr = NULL;
	char* msg_ptr = NULL;
	char* tok_ptr = NULL;

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
			retval = SIGNUP(id);

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
			if (fprintf(fp, "\n%s %s", id, pw) < 0) {
				printf("Failed to write to file");
				fclose(fp);
				return 1;
			}
			fclose(fp);
			continue;
		}

		// 1-2 : sign in
		else if (strcmp(msg, "2") == 0) {

			// Get ID, PW - Read Message from the Client
			sprintf(msg, "\033[0;36m[SYSTEM]Sign In - Enter ID\033[0m");
			send(clnt_sock, msg, strlen(msg) + 1, 0);
			if (recv(clnt_sock, id, sizeof(id), 0) == SOCKET_ERROR || strcmp(id, "quit") == 0)
				break;

			sprintf(msg, "\033[0;36m[SYSTEM]Sign In - Enter PW\033[0m");
			send(clnt_sock, msg, strlen(msg) + 1, 0);
			if (recv(clnt_sock, pw, sizeof(pw), 0) == SOCKET_ERROR || strcmp(pw, "quit") == 0)
				break;

			// Check ID & PW
			retval = SIGNIN(id, pw);

			// Incorrect ID PW
			if (retval == 1) {
				sprintf(msg, "\033[0;36m[SYSTEM]Sign In - Failed\033[0m");
				send(clnt_sock, msg, strlen(msg) + 1, 0);
				continue;
			}

			// Incorrect ID PW
			if (retval == 2) {
				sprintf(msg, "\033[0;34m[SYSTEM]Hello %s. Enter Message.\033[0m", id);
				send(clnt_sock, msg, strlen(msg) + 1, 0);
				sprintf(msg, "\033[0;36m%s join chat room\033[0m", id);
				printf("%s\n", msg);
				send_all_clnt(msg, clnt_sock);
			}

		}

		// msg : only 1 / 2
		else
			continue;


		// 2 : message
		while (1) {
			// rcv Message
			if (recv(clnt_sock, msg, sizeof(msg), 0) == SOCKET_ERROR || strcmp(msg, "quit") == 0)
				break;
			msg[strlen(msg)] = '\0';

			// whisper mode
			if (strncmp(msg, "whi", 3) == 0) {
				tok_ptr = strtok(msg, " ");
				id_ptr = strtok(NULL, " ");
				msg_ptr = strtok(NULL, " ");
				printf("%s, %s\n", id_ptr, msg_ptr);
				continue;
			}

			// send to all client
			sprintf(chat, "[%s] : %s", id, msg);
			send_all_clnt(chat, clnt_sock);
			printf("%s\n", chat);

			// Open chat File
			if ((fp = fopen("chat.txt", "a")) == NULL) {
				printf("File Open Error!\n");
				break;
			}

			// Write chat
			time_t t;
			time(&t);
			char time_str[26];
			struct tm* tm_info = localtime(&t);
			strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);

			if (fprintf(fp, "[%s] %s\n", time_str, chat) < 0) {
				printf("Failed to write to file");
				fclose(fp);
				return 1;
			}
			fclose(fp);

			// send to speaker client
			sprintf(chat, "\033[0;36m[%s] : %s\033[0m", id, msg);
			send(clnt_sock, chat, strlen(chat) + 1, 0);
		}
		// Log Out
		sprintf(msg, "\033[0;35m[%s] has left chat room\033[0m", id);
		printf("%s\n", msg);
		send_all_clnt(msg, clnt_sock);
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
