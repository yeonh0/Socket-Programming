// ErrorHandling.c
#include "ErrorHandling.h"

// Error Handling Function
void ErrorHandling(const char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
