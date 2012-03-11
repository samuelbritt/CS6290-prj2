include $(ROOT)/Rules.mk

TGT = tests

INCDIRS = ..

INCLUDES = $(INCDIRS:%=-I%)

SRC = test_*.c tests.c CuTest.c
OBJ = $(SRC:%.c=%.o)

all: $(TGT)

$(TGT): $(OBJ)
	$(LINK)

%.o: %.c
	$(COMP)

-include $(DEPS)

clean:
	rm -f $(OBJ) $(TGT)
