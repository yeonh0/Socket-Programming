// SIGNUP.c
#include "SIGNUP.h"

int SIGNUP(char* id) {
	int retval;
	int buf[2000];				// UserInfo Data (ID, PW set)
	char* tok_ptr = NULL;
	char* id_ptr = NULL;

	// ID has Space or Too long - return 1
	if (strchr(id, ' ') != NULL || strlen(id) > 15)
		return 1;

	// Open UserInfo File
	FILE* fp = fopen("info.txt", "r");
	retval = fread(buf, sizeof(char), sizeof(buf), fp);
	fclose(fp);
	buf[retval] = '\0';

	// Enter Tokenizer
	tok_ptr = strtok(buf, "\n");
	while (tok_ptr != NULL) {
		retval = strlen(tok_ptr);
		id_ptr = strtok(tok_ptr, " ");
		tok_ptr = strtok(tok_ptr + retval + 1, "\n");

		if (strcmp(id_ptr, id) == 0)
			// Duplicated ID - return 2
			return 2;
	}

	return 0;
}
