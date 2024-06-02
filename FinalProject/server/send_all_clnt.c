// send_all_clnt.c
#include "send_all_clnt.h"

// Send Message to the ALL Clients
void send_all_clnt(char* msg, SOCKET my_sock) {
	// Sending
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < g_clnt_count; i++) {
		if (my_sock != g_clnt_socks[i]) {
			send(g_clnt_socks[i], msg, strlen(msg) + 1, 0);
		}
	}
	ReleaseMutex(hMutex);
}
