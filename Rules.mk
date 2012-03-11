
CC 	= gcc
CFLAGS	= -pedantic -Wall -std=gnu99 -O2 $(DEBUG)
INCLUDES =
LDFLAGS =
LDLIBS	=
DBFLAGS = -g -O0 -DDEBUG

# Generic rules:
LINK	= $(LINK.c) -o $@ $^ $(LDLIBS)
COMP	= $(COMPILE.c) $(INCLUDES) -MMD -MP $<

