#include <stdio.h>
#include "file.h"
#include "string.h"
#include "parser/char_stream.h"

int main(int argc, char** argv) {
	if (argc != 2)
		return 1;
	char* contents = readfullfile(argv[1]);
	if (contents == NULL)
		return 2;
	printf("%d", is_letters_only("abc"));
	printf("%d", is_letters_only("1bc"));
	return 0;
}
