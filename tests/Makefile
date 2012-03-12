include $(ROOT)/Rules.mk
CFLAGS += -g -O0

TGT = tests

INCDIRS = ..

INCLUDES = $(INCDIRS:%=-I%)

SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=%.o)
DEP = $(SRC:%.c=%.d)

all: $(TGT)

$(TGT): $(OBJ)
	$(LINK)

%.o: %.c
	$(COMP)

-include $(DEP)

clean:
	@rm -f $(OBJ) $(TGT)
