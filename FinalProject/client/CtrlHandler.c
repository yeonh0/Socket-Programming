// CtrlHandler.c
#include "CtrlHandler.h"

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
    char msg[30];
    char chat[] = "quit";
    switch (fdwCtrlType) {
    case CTRL_C_EVENT:
        sprintf(msg, "\033[0;35m%s has left the room\033[0m\n", G_ID);
        printf("%s", msg);
        send(sock, chat, strlen(chat) + 1, 0);
        closesocket(sock);
        WSACleanup();
        exit(0);
        return TRUE;
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    default:
        return FALSE;
    }
}
