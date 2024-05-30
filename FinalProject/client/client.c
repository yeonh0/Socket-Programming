////////////////////////////////////////////////////////////////////////////////
// Header File
#include "client.h"

////////////////////////////////////////////////////////////////////////////////
// Main Function
int main(int argc, char* argv[])
{
	// Local Variable
	WSADATA wsa;								// WSA DATA
	struct sockaddr_in servaddr;				// Cread Server SockAddr
	HANDLE t_thread;							// Create Thread
	int retval;									// Return Value
	char chat[BUFSIZE];							// Create Chat Buffer
	char msg[1000];								// Create Sending Message Buffer

	// Check Arguments
	if (argc < 4) {
		printf("Usage : %s <IP> <PORT> <USER ID>\n", argv[0]);
		exit(1);
	}

	// INITITIALIZE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// Server Socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) ErrorHandling("ERROR: Server Socket INIT error!");

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));


	// Connecting !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	retval = connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (retval == SOCKET_ERROR) ErrorHandling("ERROR: Server Socket Connect error!");


	// Send ID to Server !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	sprintf(msg, "%s", argv[3]);
	send(sock, msg, strlen(msg) + 1, 0);
	G_ID = argv[3];

	// Set Ctrl+C handler
	retval = SetConsoleCtrlHandler(CtrlHandler, TRUE);
	if (!retval) ErrorHandling("SetConsoleCtrlHandler() Error!");

	// Create Thread !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	t_thread = CreateThread(NULL, 0, rcv, (LPVOID)sock, 0, NULL);
	if (t_thread == NULL) ErrorHandling("ERROR: Create Thread error!");

	// While  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	while (1) {
		// Get Message
		retval = 0;
		retval = fgets(chat, sizeof(chat), stdin);

		// Remove Enter
		int only_spaces = 1; // Assume the input is only spaces initially
		for (int i = 0; chat[i] != 0; i++) {
			if (chat[i] != ' ' && chat[i] != '\n') only_spaces = 0;

			if (chat[i] == '\n') {
				chat[i] = '\0';
				break;
			}
		}

		// Check if input is empty or only spaces
		if (!retval || only_spaces) {
			continue;
		}

		// quit
		if (strcmp(chat, "quit") == 0) {
			sprintf(msg, "\033[0;35m%s has left the room\033[0m\n", G_ID);
			printf("%s", msg);
			send(sock, chat, strlen(chat) + 1, 0);
			break;
		}

		// Send to Someone (Whispering)
		char whiprefix[] = "/w";	// whisper prefix : /w
		// Check if message starts with /w
		if (strncmp(chat, whiprefix, strlen(whiprefix)) == 0) {
			char* id_ptr = strtok(chat, " ");
			id_ptr = strtok(NULL, " ");							// *id_ptr = destination ID
			char* msg_ptr = strtok(NULL, " ");					// *msg_ptr = message
			if (id_ptr != NULL && msg_ptr != NULL) {			// whisper message : /w id msg
				sprintf(msg, "whi %s %s", id_ptr, msg_ptr);
				send(sock, msg, strlen(msg) + 1, 0);			// Send server "whi <id> <msg>"
			}
			else
			// print right command of whisper
				printf("\033[0;36mFail to send whisper! : /w <ID> <MSG>\n\033[0m");
			continue;
		}
		
		// Send to ALL
		sprintf(msg, "[%s] : %s\n", argv[3], chat);
		printf("\033[0;32m%s\033[0m", msg);
		send(sock, msg, strlen(msg) + 1, 0);

	}
	// close socket
	closesocket(sock);
	WSACleanup();
	return 0;
}
