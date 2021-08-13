#include <stdlib.h>
#include <string.h>
#include "string.h"
// hand written
splitstr* split(char* string, char* delimiter) {
	int targetsize = 0;
	splitstr* ret = malloc(sizeof(splitstr));
	if (ret == NULL)
		return NULL;
	ret->str = NULL;
	ret->len = 0;
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
		ret->str = realloc(ret->str, (targetsize+1) * sizeof(char*));
		ret->str[targetsize++] = newstr;
		ret->len++;
	} while (pos != NULL);
	return ret;
}

void free_splitstr(splitstr* ret) {
	for (int i = 0; i < ret->len; i++)
		free(ret->str[i]);
	free(ret);
}
