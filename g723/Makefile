# Makefile for Blackfin uClinux port of G723
# David Rowe 15 October June 2006

SUBDIRS =src.orig test

PROJ=g723
VERSION=0.1

# extract dir from /path/to/dir
RELCWD = $(shell pwd | sed -e "s/.*\///")

TEST_PROG =test 

all:
	$(MAKE) -C test

DIRS_CLEAN := $(patsubst %,%_clean,$(SUBDIRS))
clean: $(DIRS_CLEAN)
	find . '(' -name '*~' -o -name '*.s' ')' | xargs rm -f
%_clean:
	$(MAKE) -C $(patsubst %_clean,%,$@) clean

.PHONY: all  clean  test
