#include <stdio.h>
#include "file.h"
#include "string.h"
#include "parser/token_stream.h"

bool p(char c) {
	return c != 'G';
}

int main(int argc, char** argv) {
	if (argc != 2)
		return 1;
	char* contents = readfullfile(argv[1]);
	if (contents == NULL)
		return 2;
	token_stream* s = new_tk_stream(new_in_stream(contents));
	printf("%s", _tkstr_read_while(s, p));
	return 0;
}
