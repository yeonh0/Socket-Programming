// SIGNIN.c
#include "SIGNIN.h";

 int SIGNIN(char* id, char* pw) {
	int retval;
	int signinbuf[2000];				// UserInfo Data (ID, PW set)
	char* tok_ptr = NULL;
	char* id_ptr = NULL;
	char* pw_ptr = NULL;

	// ID or PW has Space or Too long - return 1
	if (strchr(id, ' ') != NULL || strlen(id) > 15 || strchr(pw, ' ') != NULL || strlen(pw) > 15)
		return 1;

	// Open UserInfo File
	FILE* file = fopen("info.txt", "r");
	if (file == NULL) {
		fprintf(stderr, "Could not open info.txt for reading\n");
		return 1;
	}

	char line[256];

	while (fgets(line, sizeof(line), file)) {
		// Remove newline character if present
		line[strcspn(line, "\n")] = 0;

		// Split the line into id and pw based on the first space
		char* id_ptr = strtok(line, " ");
		char* pw_ptr = strtok(NULL, " ");

		if (id_ptr && pw_ptr) {
			if ((strcmp(id, id_ptr) == 0) && (strcmp(pw, pw_ptr) == 0)) {
				return 2;
			}
		}
	}

	fclose(file);
	return 1;
}
