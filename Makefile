INCDIR   := include
SRCDIR   := src
BUILDDIR := build

CC       := gcc
CFLAGS   := -Wall -Wextra -Wpedantic -std=c11 -I$(INCDIR)
DEBUG    ?= no

ifeq ($(DEBUG),yes)
	CFLAGS += -O0 -g
	TYPE   := debug
else
	CFLAGS += -O2
	TYPE   := release
endif

LIB  := $(BUILDDIR)/$(TYPE)/libimgproc.a

SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(addprefix $(BUILDDIR)/$(TYPE)/,$(SRCS:.c=.o))
DEPS := $(OBJS:.o=.d)

AR := ar rcs
RM := rm -rf

.PHONY: all clean

all: $(LIB) $(TEST)

-include $(DEPS)

$(LIB): $(OBJS)
	$(AR) $@ $^

$(BUILDDIR)/$(TYPE)/%.o: %.c
	@mkdir -p $(BUILDDIR)/$(TYPE)/$(SRCDIR)
	$(CC) $(CFLAGS) -c -MMD -MP $< -o $@

clean:
	$(RM) $(BUILDDIR)
