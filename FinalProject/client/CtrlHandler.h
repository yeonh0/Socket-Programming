// CtrlHandler.h
#ifndef CTRL_HANDLER_H
#define CTRL_HANDLER_H

#include "client.h"

extern SOCKET sock;
extern char* g_ID;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

#endif // CTRL_HANDLER_H
