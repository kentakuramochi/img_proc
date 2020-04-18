INCDIR   := include
SRCDIR   := src
BUILDDIR := build
SAMPDIR  := sample

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

SAMPSRCS := $(wildcard $(SAMPDIR)/*.c)
SAMPS    := $(addprefix $(BUILDDIR)/$(TYPE)/,$(SAMPSRCS:.c=))

AR := ar rcs
RM := rm -rf

.PHONY: all test clean

all: test 

-include $(DEPS)

test: $(SAMPS)
	@mkdir -p $(BUILDDIR)/$(TYPE)/sample/out
	@for i in ./$(SAMPS); do echo $$i; done

$(LIB): $(OBJS)
	$(AR) $@ $^

$(BUILDDIR)/$(TYPE)/%.o: %.c
	@mkdir -p $(BUILDDIR)/$(TYPE)/$(SRCDIR)
	$(CC) $(CFLAGS) -c -MMD -MP $< -o $@

$(SAMPS): $(LIB)

$(BUILDDIR)/$(TYPE)/$(SAMPDIR)/%: $(SAMPDIR)/%.c
	@mkdir -p $(BUILDDIR)/$(TYPE)/$(SAMPDIR)
	$(CC) $(CFLAGS) -L./$(BUILDDIR)/$(TYPE) $< -limgproc $(LIBS) -o $@

clean:
	$(RM) $(BUILDDIR)
