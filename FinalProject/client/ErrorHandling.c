// ErrorHandling.c
#include "ErrorHandling.h"
#include <stdio.h>
#include <stdlib.h>

// Error Handling Function
void ErrorHandling(const char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
