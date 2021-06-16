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

struct split_string {
	int len;
	char** str;
};
typedef struct split_string splitstr;
// hand written
splitstr* split(char* string, char* delimiter) {
	int targetsize = 0;
	splitstr* ret = malloc(sizeof(splitstr));
	if (ret == NULL)
		return NULL;
	ret->str = NULL;
	char* pos;
	char* oldpos = string;
	int newsize;
	int dlen = strlen(delimiter);
	do {
		pos = strstr(oldpos, delimiter);
		if (pos) {
			newsize = pos - oldpos;
		} else {
			newsize = strlen(oldpos);
		}
		char* newstr = malloc(sizeof(char) * (newsize + 1));
		strncpy(newstr, oldpos, newsize);
		newstr[newsize] = '\0';
		oldpos = pos + dlen;
		printf("Found %s\n", newstr);
		ret->str = realloc(ret->str, (targetsize+1) * sizeof(char*));
		ret->str[targetsize++] = newstr;
		//ret->len++;
	} while (pos != NULL);
	for (int i = 0; i < ret->len; i++) {
		printf("Element %d: %s", i, ret->str[i]);
	}
	return ret;
}

int main(int argc, char** argv) {
	if (argc != 2)
		return 1;
	char* contents = readfullfile(argv[1]);
	if (contents == NULL)
		return 2;
	printf("%s", contents);
	split(contents, "a");
	return 0;
}
