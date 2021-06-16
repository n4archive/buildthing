OUT ?= out
GCC ?= gcc
LDFLAGS ?=
CFLAGS ?= -Wall -Wextra -g
CPPFLAGS ?= -DBUILDTHING
CSRCS ?= main.c
COBJS = $(addprefix $(OUT)/, $(CSRCS:.c=.o))
OBJS = $(COBJS)

DEFAULT: out/buildthing

clean:
	rm -rf $(OUT)

$(OUT)/%.o: %.c
	mkdir -p $(OUT)
	$(GCC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OUT)/buildthing: $(OBJS)
	$(GCC) -o $@ $(OBJS) $(LDFLAGS)
