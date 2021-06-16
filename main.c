#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char** argv) {
	if (argc != 2)
		return 1;
	char* contents = readfullfile(argv[1]);
	if (contents == NULL)
		return 2;
	printf("%s", contents);
	return 0;
}
