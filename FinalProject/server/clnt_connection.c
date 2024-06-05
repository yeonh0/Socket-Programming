// clnt_connection.c
#include "clnt_connection.h"

DWORD WINAPI clnt_connection(LPVOID arg) {
	// Local Variable
	SOCKET clnt_sock = (SOCKET)arg;		// Client Socket
	int str_len = 0;					// Message Length
	char msg[BUFSIZE];					// Message Buffer
	char chat[BUFSIZE];
	char id[100], pw[100];				// Client ID, PW
	int retval;
	
	FILE* fp;
	time_t t;
	char time_str[26];
	struct tm* tm_info;

	char* id_ptr = NULL;				// Whisper Dest, Msg, idchk
	char* msg_ptr = NULL;

	char joinmsg[BUFSIZE];
	int roomnum_client = 100;
	char roomname_client[30];

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
			if ((recv(clnt_sock, id, sizeof(id), 0) == SOCKET_ERROR) || strcmp(id, "quit") == 0)
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
			if ((recv(clnt_sock, pw, sizeof(pw), 0) == SOCKET_ERROR) || strcmp(pw, "quit") == 0)
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
			if ((recv(clnt_sock, id, sizeof(id), 0) == SOCKET_ERROR) || strcmp(id, "quit") == 0)
				break;

			sprintf(msg, "\033[0;36m[SYSTEM]Sign In - Enter PW\033[0m");
			send(clnt_sock, msg, strlen(msg) + 1, 0);
			if ((recv(clnt_sock, pw, sizeof(pw), 0) == SOCKET_ERROR) || strcmp(pw, "quit") == 0)
				break;

			// Check ID & PW
			retval = SIGNIN(id, pw);

			// Incorrect ID PW
			if (retval == 1) {
				sprintf(msg, "\033[0;36m[SYSTEM]Sign In - Failed\033[0m");
				send(clnt_sock, msg, strlen(msg) + 1, 0);
				continue;
			}

			// Correct ID PW
			if (retval == 2) {
				retval = 0;
				// Check Duplicate Connecting
				WaitForSingleObject(hMutex, INFINITE);
				for (int i = 0; i < g_log_count; i++) {
					if (strcmp(id, g_connect_list[i].con_id) == 0) {
						sprintf(msg, "\033[0;36m[SYSTEM]Account is already being linked\033[0m");
						send(clnt_sock, msg, strlen(msg) + 1, 0);
						retval = 1;
						break;
					}
				}
				ReleaseMutex(hMutex);

				if (retval == 1) continue;

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

		// Add Client list
		WaitForSingleObject(hMutex, INFINITE);
		strcpy(g_connect_list[g_log_count].con_id, id);
		g_connect_list[g_log_count].con_sock = clnt_sock;
		g_connect_list[g_log_count].roomnum = 100;
		printf("New : %s, total : %d\n", g_connect_list[g_log_count].con_id, g_log_count+1);
		g_log_count++;
		ReleaseMutex(hMutex);

		// 2 : message
		while (1) {
			// 2-0. rcv Message
			if (recv(clnt_sock, msg, sizeof(msg), 0) == SOCKET_ERROR || strcmp(msg, "quit") == 0)
				break;
			msg[strlen(msg)] = '\0';

			// 2-1. Whisper Mode
			if (strncmp(msg, "whi", 3) == 0) {
				strtok(msg, " ");
				id_ptr = strtok(NULL, " ");
				msg_ptr = id_ptr + strlen(id_ptr) + 1;

				// Search destination ID in connect clients list
				WaitForSingleObject(hMutex, INFINITE);
				for (int i = 0; i < g_log_count; i++) {
					if ((strcmp(id_ptr, g_connect_list[i].con_id) == 0) && strcmp(id_ptr, id) != 0) {
						// Destination Client Log in - Send Msg
						printf("%s -> %s : %s\n", id, id_ptr, msg_ptr);
						sprintf(chat, "\033[0;36m[SEND to %s] : %s\033[0m", id_ptr, msg_ptr);
						send(clnt_sock, chat, strlen(chat) + 1, 0);

						sprintf(chat, "\033[0;36m[FROM %s] : %s\033[0m", id, msg_ptr);
						send(g_connect_list[i].con_sock, chat, strlen(chat) + 1, 0);

						break;
					}
				}
				ReleaseMutex(hMutex);
				continue;
			}

			// 2-2. Chat Room Options - (1) Make Room, (2) Search Room, (3) Join Room
			// (1). Make Room
			if (strncmp(msg, "mkrm", 4) == 0) {
				retval = 0;
				// mkrm hihi
				strtok(msg, " ");
				id_ptr = strtok(NULL, " ");					// room num

				// room name check 
				if (id_ptr == NULL || strlen(id_ptr) > 15) {
					sprintf(chat, "\033[0;36mInvalid Room Name!\033[0m");
					send(clnt_sock, chat, strlen(chat) + 1, 0);
					continue;
				}

				// check duplicate room & room count
				WaitForSingleObject(hMutex, INFINITE);
				for (int i = 0; i < g_room_count; i++) {
					if ((strcmp(id_ptr, g_room_name[i]) == 0) || g_room_count == MAXROOM) {
						// Destination Client Log in - Send Msg
						sprintf(chat, "\033[0;36mCan Not Make Room! (Total Room : %d)\033[0m", g_room_count);
						send(clnt_sock, chat, strlen(chat) + 1, 0);
						retval = 1;
						break;
					}
				}
				ReleaseMutex(hMutex);

				if (retval == 1) continue;

				// Make Room : name id_ptr
				WaitForSingleObject(hMutex, INFINITE);
				strcpy(g_room_name[g_room_count], id_ptr);
				g_room_count++;
				sprintf(chat, "\033[0;36mRoom %s is opened. Total Room : %d\033[0m", id_ptr, g_room_count);
				ReleaseMutex(hMutex);
				send(clnt_sock, chat, strlen(chat) + 1, 0);
				continue;
			}

			// (2). Search Room
			if (strncmp(msg, "srrm", 4) == 0) {
				// check duplicate room & room count
				WaitForSingleObject(hMutex, INFINITE);
				sprintf(chat, "\033[0;36mTotal Room(%d) : ", g_room_count);

				for (int i = 0; i < g_room_count; i++) {
					strcat(chat, g_room_name[i]);

					if (i < g_room_count - 1) {
						strcat(chat, ", ");
					}
				}
				strcat(chat, "\033[0m");
				ReleaseMutex(hMutex);

				send(clnt_sock, chat, strlen(chat) + 1, 0);

				continue;
			}

			// (3). Join Room
			if (strncmp(msg, "goto", 4) == 0) {
				retval = -1;
				// find room exist
				strtok(msg, " ");
				id_ptr = strtok(NULL, " ");					// room num

				WaitForSingleObject(hMutex, INFINITE);
				
				// If id_ptr is exist : retval = room num
				for (int i = 0; i < g_room_count; i++) {
					if (strcmp(id_ptr, g_room_name[i]) == 0) {
						retval = i;
						break;
					}
				}

				// If room exist : change client roomnum, find room member
				if (retval != -1) {
					roomnum_client = retval;
					sprintf(joinmsg, "\033[0;36mJoin Member : ");
					for (int i = 0; i < g_log_count; i++) {

						// Change Client Room Num
						if (strcmp(id, g_connect_list[i].con_id) == 0) {
							g_connect_list[i].roomnum = retval;
							sprintf(chat, "\033[0;36mJOIN into Room - %s\033[0m", g_room_name[g_connect_list[i].roomnum]);
							send(clnt_sock, chat, strlen(chat) + 1, 0);
							strcpy(roomname_client, g_room_name[g_connect_list[i].roomnum]);
						}

						// Room Member Find
						if (g_connect_list[i].roomnum == retval) {
							strcat(joinmsg, g_connect_list[i].con_id);
							strcat(joinmsg, " ");
						}
					}
					strcat(joinmsg, "\033[0m");
					send(clnt_sock, joinmsg, strlen(joinmsg) + 1, 0);
				}

				// If room doesn't exist
				else {
					sprintf(chat, "\033[0;36mRoom %s Doesn't Exist!\033[0m", id_ptr);
					send(clnt_sock, chat, strlen(chat) + 1, 0);
				}
				ReleaseMutex(hMutex);
				continue;
			}

			// 3. Chat mode
			// (1). Send ALL - Prefix = 'toall' or Not yet Joining Any Room

			// prefix = toall or not join room : send to all client
			if (strncmp(msg, "toall", 5) == 0) {
				id_ptr = strtok(msg, " ");
				msg_ptr = id_ptr + 6;

				if (msg_ptr != NULL) {
					sprintf(chat, "\033[0;33m[%s] : %s\033[0m", id, msg_ptr);
					send_all_clnt(chat, clnt_sock);
					printf("%s\n", chat);
				}

				// send to speaker client
				sprintf(chat, "\033[0;32m[%s] : %s\033[0m", id, msg_ptr);
				send(clnt_sock, chat, strlen(chat) + 1, 0);
			}

			// yet join room
			else if (roomnum_client == 100) {
				sprintf(chat, "\033[0;33m[%s] : %s\033[0m", id, msg);
				send_all_clnt(chat, clnt_sock);
				printf("%s\n", chat);

				// send to speaker client
				sprintf(chat, "\033[0;32m[%s] : %s\033[0m", id, msg);
				send(clnt_sock, chat, strlen(chat) + 1, 0);
			}

			// (2). else : send to room members
			else {
				sprintf(chat, "\033[0;35m[%s] : %s\033[0m", id, msg);
				printf("%s - %s\n", roomname_client, chat);

				WaitForSingleObject(hMutex, INFINITE);
				for (int i = 0; i < g_log_count; i++) {
					// If not this client program and same room number
					if (clnt_sock != g_connect_list[i].con_sock && g_connect_list[i].roomnum == roomnum_client) {
						send(g_clnt_socks[i], chat, strlen(chat) + 1, 0);
					}
				}
				ReleaseMutex(hMutex);

				// send to speaker client
				sprintf(chat, "\033[0;32m[%s] : %s\033[0m", id, msg);
				send(clnt_sock, chat, strlen(chat) + 1, 0);
			}

			// 4. Save Chat Data
			// Open chat File
			if ((fp = fopen("chat.txt", "a")) == NULL) {
				printf("File Open Error!\n");
				break;
			}

			// Write chat
			time(&t);
			tm_info = localtime(&t);
			strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);

			if (fprintf(fp, "[%s] %s\n", time_str, chat) < 0) {
				printf("Failed to write to file");
				fclose(fp);
				return 1;
			}
			fclose(fp);
		}
		// 5. Log Out
		if(id != NULL) {
			sprintf(msg, "\033[0;35m[%s] has left chat room\033[0m", id);
			printf("%s\n", msg);
			send_all_clnt(msg, clnt_sock);
			break;
		}
	}

	WaitForSingleObject(hMutex, INFINITE);
	// Remove the client socket from the socket list
	for (int i = 0; i < g_clnt_count; i++) {
		if (clnt_sock == g_clnt_socks[i]) {
			for (; i < g_clnt_count - 1; i++)
				g_clnt_socks[i] = g_clnt_socks[i + 1];
			break;
		}
	}
	g_clnt_count--;

	// Remove the login client from the losgin client list
	for (int i = 0; i < g_log_count; i++) {
		if (strcmp(id, g_connect_list[i].con_id) == 0) {
			printf("Disappear : %s, total : %d\n", g_connect_list[i].con_id, g_log_count - 1);
			for (; i < g_log_count - 1; i++)
				g_connect_list[i] = g_connect_list[i + 1];
			g_log_count--;
			break;
		}
	}
	ReleaseMutex(hMutex);

	// Close the Client Socket
	closesocket(clnt_sock);

	// Exit Thread
	ExitThread(0);
	return 0;
}
