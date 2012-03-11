# This file hopefully should not need to be modified often

export ROOT = $(shell pwd)

include Rules.mk
include sources.mk

BINDIR	= .
TESTDIR	= tests

SIM_TGT = $(BINDIR)/tomasulo_sim
SIM_OBJS = $(SIM_SRCS:%.c=%.o)
SIM_DEPS = $(SIM_SRCS:%.c=%.d)

TGTS = $(SIM_TGT)
SRCS = $(SIM_SRCS)
OBJS = $(SIM_OBJS)
DEPS = $(SIM_DEPS)

all: $(TGTS)

$(SIM_TGT): $(SIM_OBJS)
	@mkdir -p $(BINDIR)
	$(LINK)
%.o: %.c
	$(COMP)

-include $(DEPS)

debug:
	@$(MAKE) clean; $(MAKE) DEBUG="$(DBFLAGS)"

test:
	@$(MAKE); $(MAKE) -C $(TESTDIR); $(TESTDIR)/tests;

clean:
	@rm -rf $(OBJS) $(DEPS) $(TGTS);
	@if [ -d $(TESTDIR) ] ; then \
		$(MAKE) -C $(TESTDIR) clean; \
	fi

.PHONY: all clean
