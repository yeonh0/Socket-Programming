// CtrlHandler.c
#include "CtrlHandler.h"

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
    char msg[40];
    switch (fdwCtrlType) {
    case CTRL_C_EVENT:
        sprintf(msg, "\033[0;35mGOOD BYE\033[0m");
        printf("%s\n", msg);
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
