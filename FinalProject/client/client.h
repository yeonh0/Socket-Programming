// client.h
#ifndef CLIENT_H
#define CLIENT_H

////////////////////////////////////////////////////////////////////////////////
// Header File
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include "ErrorHandling.h"
#include "rcv.h"
#include "CtrlHandler.h"

#pragma comment(lib, "ws2_32")

////////////////////////////////////////////////////////////////////////////////
// User Define
#define BUFSIZE    100		// Buffer Size
#define NAMESIZE   20		// Max Client

////////////////////////////////////////////////////////////////////////////////
// Global Variable
SOCKET sock;								// Create Server Socket
char* G_ID;									// User ID

#endif // CLIENT_H