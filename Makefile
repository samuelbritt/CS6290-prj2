# This file hopefully should not need to be modified often

export ROOT = $(shell pwd)

include Rules.mk

SRCDIR  = src
BINDIR	= .
BUILDDIR = build
TESTDIR	= tests

SIM_TGT = $(BINDIR)/tomasulo_sim
SIM_SRCS = $(wildcard $(SRCDIR)/*.c)
SIM_OBJS = $(SIM_SRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
SIM_DEPS = $(SIM_OBJS:%.o=%.d)

TGTS = $(SIM_TGT)
SRCS = $(SIM_SRCS)
OBJS = $(SIM_OBJS)
DEPS = $(SIM_DEPS)

all: $(TGTS)

$(SIM_TGT): $(SIM_OBJS)
	@mkdir -p $(BINDIR)
	$(LINK)
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(COMP)

-include $(DEPS)

debug:
	@$(MAKE) clean; $(MAKE) DEBUG="$(DBFLAGS)"

test:
	@$(MAKE); $(MAKE) -C $(TESTDIR); $(TESTDIR)/validation.py; $(TESTDIR)/tests;

clean:
	@rm -rf $(BUILDDIR) $(TGTS);
	@if [ -d $(TESTDIR) ] ; then \
		$(MAKE) -C $(TESTDIR) clean; \
	fi

dist:
	git arch britt_cs6290_prj2

.PHONY: all clean dist
