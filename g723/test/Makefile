# Makefile for g723/test
#
# Top level Makefile that builds all libraries and test programs
#FDPIC_CC = bfin-linux-uclibc-gcc

#FLAT_CC = gcc
FLAT_CC = bfin-uclinux-gcc

G723_CFLAGS += -I ../include/ -Wall

#FLAT_LDFLAGS += -pipe -Wall -g -O2
FLAT_LDFLAGS += -pipe -Wall -O2 -Wl,-elf2flt=-s100000 -mcpu=bf533-0.5 -g -DEMBED 


PROGS := lbccodec tvcmp

all: $(PROGS)

../src.%:
	$(MAKE) -C $(patsubst %/libg723.a,%,$@)

# flat test program

lbccodec.o: lbccodec.c 
	$(FLAT_CC) $(G723_CFLAGS)  -o lbccodec.o -c lbccodec.c

lbccodec: lbccodec.o ../src.orig/libg723.a
	$(FLAT_CC) $(FLAT_LDFLAGS) -o $@ $^ 

tvcmp:compare_test.c
	gcc -o tvcmp compare_test.c

clean:
	rm -f *.o $(PROGS) *.gdb *~ \#*

.PHONY: all clean $(LIBDIRS)
