OUT ?= out
GCC ?= gcc
R2C ?= re2c
LDFLAGS ?=
CFLAGS ?= -Wall -Wextra -g -Og
CPPFLAGS ?= -DBUILDTHING
R2CSRCS := parser/regex.r2c
CSRCS := main.c file.c string.c parser/char_stream.c parser/token_stream.c parser/parser.c parser/debug_ast.c interpreter/interpreter.c interpreter/debug_any.c
CSRCS += $(addprefix $(OUT)/, $(R2CSRCS:.r2c=.c))
COBJS = $(addprefix $(OUT)/, $(CSRCS:.c=.o))
OBJS = $(COBJS)

DEFAULT: out/buildthing

clean:
	rm -rf $(OUT)

$(OUT)/%.c: %.r2c
	mkdir -p `dirname $@`
	$(R2C) $< -o $@

$(OUT)/%.o: %.c
	mkdir -p `dirname $@`
	$(GCC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OUT)/buildthing: $(OBJS)
	$(GCC) -o $@ $(OBJS) $(LDFLAGS)
