#ifndef STRING_H
#define STRING_H
struct _split_string {
	int len;
	char** str;
};
typedef struct _split_string splitstr;
splitstr* split(char* string, char* delimiter);
#endif
