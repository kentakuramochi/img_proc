INCDIR   := include
SRCDIR   := src
BUILDDIR := build
TESTDIR  := test

CC       := gcc
CFLAGS   := -Wall -Wextra -Wpedantic -std=c11 -I./$(INCDIR)
LIBS     := -lm
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

TESTSRCS := $(wildcard $(TESTDIR)/*.c)
TESTS    := $(addprefix $(BUILDDIR)/$(TYPE)/,$(TESTSRCS:.c=))

AR := ar rcs
RM := rm -rf

.PHONY: all test clean

all: test

-include $(DEPS)

$(LIB): $(OBJS)
	$(AR) $@ $^

$(BUILDDIR)/$(TYPE)/%.o: %.c
	@mkdir -p $(BUILDDIR)/$(TYPE)/$(SRCDIR)
	$(CC) $(CFLAGS) -c -MMD -MP $< -o $@

test: $(LIB) $(TESTS)

$(BUILDDIR)/$(TYPE)/$(TESTDIR)/%: $(TESTDIR)/%.c
	@mkdir -p $(BUILDDIR)/$(TYPE)/$(TESTDIR)
	$(CC) $(CFLAGS) -L./$(BUILDDIR)/$(TYPE) $< -limgproc $(LIBS) -o $@

clean:
	$(RM) $(BUILDDIR)
