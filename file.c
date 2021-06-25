#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readfullfile(char* name) {	
	FILE *f = fopen(name, "rb");
	if (f == NULL)
		return NULL;
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	rewind(f);
	char *string = malloc(fsize + 1);
	if (string == NULL) {
		fclose(f);
		return NULL;
	}
	fread(string, 1, fsize, f);
	fclose(f);
	string[fsize] = 0;
	return string;
}
