// SIGNIN.h
#ifndef SIGNIN_H
#define SIGNIN_H

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

int SIGNIN(char* id, char* pw);

#endif // SIGNIN_H