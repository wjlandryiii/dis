CC=gcc
CFLAGS=-lcapstone -lreadline -g -Wall

all: test.exe dis_helloworld.exe

bytechunk.o:	bytechunk.c bytechunk.h bytefields.h error.h
bytes.o: 	bytes.c bytes.h bytechunk.h bytefields.h error.h
workspace.o: 	workspace.c workspace.h
testrunner.o: 	testrunner.c testrunner.h
errno.o: 	errno.c errno.h
loader.o:	loader.c loader-elf.c loader-raw.c loader.h
lines.o:	lines.c lines.h workspace.h bytes.h


CORE_OBJS = error.o \
	    bytechunk.o \
	    bytes.o \
	    workspace.o \
	    disassembler.o \
	    loader.o \
	    lines.o \

core.a: $(CORE_OBJS)
	ar rcs $@ $^

TEST_OBJS = bytechunk-test.o \
	    bytes-test.o \
	    workspace-test.o \
	    disassembler-test.o \
	    loader-test.o \
	    lines-test.o

test.exe: testrunner.o $(TEST_OBJS) core.a
	$(CC) $(CFLAGS) -o $@ $^

dis_helloworld.exe: dis_helloworld.o core.a
	$(CC) $(CFLAGS) -o $@ $^

dis_helloworld.o: dis_helloworld.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.exe
	rm -f *.a
