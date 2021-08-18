#include "token_stream.h"
#include "regex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_stream *new_tk_stream(input_stream *input) {
  token_stream *s = malloc(sizeof(token_stream));
  s->instr = input;
  s->current = NULL;
  s->failed = false;
  s->eof = false;
  return s;
}

void sync_tkstr_fail(token_stream *s) {
  if (s->instr->failed)
    s->failed = true;
  if (s->instr->eof)
    s->eof = true;
}

token *tkstr_next(token_stream *s) {
  sync_tkstr_fail(s);
  if (s->eof)
    return NULL;
  token *t = s->current;
  s->current = NULL;
  if (t)
    return t;
  return _tkstr_read_next(s);
}

token *tkstr_peek(token_stream *s) {
  sync_tkstr_fail(s);
  if (s->eof)
    return NULL;
  if (s->current)
    return s->current;
  return (s->current = _tkstr_read_next(s));
}

void tkstr_fail(token_stream *s, char *fail) {
  s->failed = true;
  instr_fail(s->instr, fail);
}

char *_tkstr_read_while_p(token_stream *s, void *ref,
                          bool (*predicate)(char, void *)) {
  int size = 1000;
  int free = size;
  char *final = malloc(sizeof(char) * size);
  while (!s->eof && !s->failed && predicate(instr_peek(s->instr), ref)) {
    sync_tkstr_fail(s);
    final[size - (free--)] = instr_next(s->instr);
    if (free < 1) {
      size += 100;
      final = realloc(final, size * sizeof(char));
    }
  }
  final[size - free] = '\0';
  return final;
}

char *_tkstr_read_while(token_stream *s, bool (*predicate)(char)) {
  int size = 1000;
  int free = size;
  char *final = malloc(sizeof(char) * size);
  while (!s->eof && !s->failed && predicate(instr_peek(s->instr))) {
    sync_tkstr_fail(s);
    final[size - (free--)] = instr_next(s->instr);
    if (free < 1) {
      size += 100;
      final = realloc(final, size * sizeof(char));
    }
  }
  final[size - free] = '\0';
  return final;
}

char *_tkstr_read_escaped(token_stream *s, char end) {
  int size = 1000;
  int free = size;
  bool escaped = false;
  char *final = malloc(sizeof(char) * size);
  char c = '\0';
  while (!s->eof && !s->failed) {
    c = instr_next(s->instr);
    sync_tkstr_fail(s);
    if (escaped) {
      if (c == 'a') {
        final[size - (free--)] = '\a';
      } else if (c == 'b') {
        final[size - (free--)] = '\b';
      } else if (c == 'f') {
        final[size - (free--)] = '\f';
      } else if (c == 'n') {
        final[size - (free--)] = '\n';
      } else if (c == 'r') {
        final[size - (free--)] = '\r';
      } else if (c == 't') {
        final[size - (free--)] = '\t';
      } else if (c == 'v') {
        final[size - (free--)] = '\v';
      } else {
        final[size - (free--)] = c;
      }
      escaped = false;
    } else if (c == '\\') {
      escaped = true;
    } else if (c == end) {
      break;
    } else {
      final[size - (free--)] = c;
    }
    if (free < 1) {
      size += 100;
      final = realloc(final, size * sizeof(char));
    }
  }
  final[size - free] = '\0';
  if (c != end)
    tkstr_fail(s, "unterminated string");
  return final;
}

bool _tkstr_predreadnum(char c, void *ap) {
  int *a = (int *)ap;
  if (c == '.') {
    if (a[1])
      return false;
    a[1] = 1;
    return true;
  }
  if (c == '0' && a[0] == 0) {
    a[0] = 1;
    return true;
  }
  if (a[0] == 0) {
    a[0] = -1;
  } else if (a[0] == 1 && c == 'x') {
    a[0] = 2;
    return true;
  } else if (a[0] == 2) {
    return is_hex_digit(c);
  }
  return is_digit(c);
}

// TODO: Negative numbers (they should not be implemented in read_number, the
// negation should be computed after all tokens have been read (e.g. combine:
// [12] [/] [-] [2] -> [12] [/] [-2])
token *_tkstr_read_number(token_stream *s) {
  int ia[2];
  char *numStr = _tkstr_read_while_p(s, ia, _tkstr_predreadnum);
  double number;
  token *t = malloc(sizeof(token));

  // Parses the number
  if (ia[0] != 2)
    sscanf(numStr, "%lf", &number);
  else
    number = (int)strtol(numStr, NULL, 0);

  // Checks if the hex value is invalid
  if (number == -1 && ia[0] == 2) {
    tkstr_fail(s, numStr);
    return NULL;
  }

  // Saves the number to the token
  t->raw = numStr;
  t->numberValue = number;
  t->type = NUMBER;

  return t;
}

token *_tkstr_read_ident(token_stream *s) {
  char *id = _tkstr_read_while(s, is_id);
  token *t = malloc(sizeof(token));
  t->raw = id;
  t->type = is_keyword(id) ? KEYWORD : VAR;
  return t;
}

token *_tkstr_read_string(token_stream *s) {
  token *t = malloc(sizeof(token));
  instr_next(s->instr);
  t->raw = _tkstr_read_escaped(s, '"');
  t->type = STRING;
  return t;
}

void _tkstr_skip_comment(token_stream *s) {
  _tkstr_read_while(s, is_not_newline);
  instr_next(s->instr);
}

token *_tkstr_read_next(token_stream *s) {
  _tkstr_read_while(s, is_whitespace);
  sync_tkstr_fail(s);
  if (s->eof)
    return NULL;

  char c = instr_peek(s->instr);
  if (c == '\0') {
    instr_next(s->instr);
    sync_tkstr_fail(s);
    return NULL;
  }
  if (c == '#') {
    _tkstr_skip_comment(s);
    return _tkstr_read_next(s);
  }
  if (c == '"')
    return _tkstr_read_string(s);
  if (is_digit(c))
    return _tkstr_read_number(s);
  if (is_id_start(c))
    return _tkstr_read_ident(s);
  if (is_punc(c)) {
    token *t = malloc(sizeof(token));
    t->raw = char2string(instr_next(s->instr));
    t->type = PUNC;
    return t;
  }
  if (is_op_char(c)) {
    token *t = malloc(sizeof(token));
    t->raw = _tkstr_read_while(s, is_op_char);
    t->type = OP;
    return t;
  }
  char err[35] = {' '};
  snprintf(err, 35, "Can't handle char (%d)", (int)c);
  tkstr_fail(s, err);
  return NULL;
}

void tkstr_free(token_stream *s) {
  if (s->instr)
    instr_free(s->instr);
  if (s->current)
    free(s->current);
  free(s);
}

void destroy_token(token *t) {
  if (t->raw)
    free(t->raw);
  free(t);
}
