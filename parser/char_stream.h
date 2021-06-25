#ifndef CHAR_STREAM_H
#define CHAR_STREAM_H
#include <stdbool.h>
struct _input_stream_struct {
	long pos;
	long line;
	long col;
	long len;
	bool failed;
	bool eof;
	char* _data;
};
typedef struct _input_stream_struct input_stream;

input_stream* new_in_stream(char* input);
char instr_next(input_stream* s);
char instr_peek(input_stream* s);
void instr_fail(input_stream* s, char* fail);
#endif
