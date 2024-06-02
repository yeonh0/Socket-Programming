// rcv.c
#include "rcv.h"

DWORD WINAPI rcv(LPVOID arg) {
	// Local Variable
	SOCKET sock = (SOCKET)arg;		// Client Socket
	int str_len = 0;					// Message Length
	char msg[BUFFSIZE];					// Message Buffer

	// While
	while (1) {
		str_len = recv(sock, msg, sizeof(msg), 0);
		if (str_len == -1) break;

		// Remove Enter
		for (int i = 0; msg[i] != 0; i++) {
			if (msg[i] == '\n') {
				msg[i] = '\0';
				break;
			}
		}

		printf("%s\n", msg);
	}

	// Thread Exit
	ExitThread(0);
	return 0;
}