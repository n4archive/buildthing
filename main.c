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
	while (!s->eof) {
		token* n = tkstr_next(s);
		if (n == NULL && !s->eof)
			printf("Error?\n");
		else if (n != NULL) {
			printf("Type: %d | Content: %s | Numeric value: %f\n", n->type, n->raw, n->numberValue);
			destroy_token(n);
		}
	}
	tkstr_free(s);
	return 0;
}
