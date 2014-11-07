CC=gcc
CFLAGS=-lcapstone -lreadline -g -Wall

all: test.exe

CORE_OBJS = error.o \
	    bytechunk.o \
	    bytes.o \
	    workspace.o \
	    disassembler.o \
	    loader.o

core.a: $(CORE_OBJS)
	ar rcs $@ $^

TEST_OBJS = bytes-test.o \
	    workspace-test.o \
	    disassembler-test.o \
	    loader-test.o \

test.exe: testrunner.o $(TEST_OBJS) core.a
	$(CC) $(CFLAGS) -o $@ $^

bytes.o: bytes.c bytes.h
workspace.o: workspace.c workspace.h
testrunner.o: testrunner.c testrunner.h
errno.o: errno.c errno.h

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.exe
	rm -f *.a
