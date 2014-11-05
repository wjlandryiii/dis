CC=gcc
CFLAGS=-lcapstone -lreadline -g -Wall

all:	dis \
	stringstore-test \
	mapping-test \
	lines-test \
	bytes-test.exe \
	bytes-interact.exe \
	interact-test.exe


tests:	workspace-test.exe \
	loader-test.exe

loader-test.exe: loader-test.o loader.o workspace.o testrunner.o bytes.o
	$(CC) $(CFLAGS) -o $@ $^

loader-test.o: loader-test.c
	$(CC) $(CFLAGS) -c -o $@ $<

workspace-test.exe: workspace-test.o workspace.o testrunner.o bytes.o
	$(CC) $(CFLAGS) -o $@ $^

testrunner.o: testrunner.c testrunner.h
	$(CC) $(CFLAGS) -c -o $@ $<

workspace-test.o: workspace-test.c
	$(CC) $(CFLAGS) -c -o $@ $<

workspace.o: workspace.c workspace.h
	$(CC) $(CFLAGS) -c -o $@ $<

loader.o: loader.c loader-raw.c loader-elf.c loader.h
	$(CC) $(CFLAGS) -c -o $@ $<

bytes-interact.exe: bytes-interact.o interact.o bytes.o
	$(CC) $(CFLAGS) -o $@ $^

bytes-interact.o: bytes-interact.c bytes.h interact.o
	$(CC) $(CFLAGS) -c -o $@ $<

interact-test.exe: interact-test.o interact.o
	$(CC) $(CFLAGS) -o $@ $^

interact-test.o: interact-test.c interact.h
	$(CC) $(CFLAGS) -c -o $@ $<

interact.o: interact.c interact.h
	$(CC) $(CFLAGS) -c -o $@ $<


bytes.o: 	bytes.c bytes.h
	$(CC) $(CFLAGS) -c -o $@ $<

bytes-test.exe: bytes-test.o bytes.o
	$(CC) $(CFLAGS) -o $@ $^

bytes-test.o: bytes-test.c bytes.h
	$(CC) $(CFLAGS) -c -o $@ $<

dis:		dis.c
	$(CC) $(CFLAGS) -o $@ $<

makedb: 	makedb.o bytes.o
	$(CC) $(CFLAGS) -o $@ $^

makedb.o:	makedb.c bytes.h
	$(CC) $(CFLAGS) -c -o $@ $<

printdb: 	printdb.o bytes.o
	$(CC) $(CFLAGS) -o $@ $^

printdb.o:	printdb.c bytes.h
	$(CC) $(CFLAGS) -c -o $@ $<

fakedis: fakedis.o bytes.o
	$(CC) $(CFLAGS) -o $@ $^

fakedis.o: fakedis.c bytes.h
	$(CC) $(CFLAGS) -c -o $@ $<

stringstore-test: stringstore-test.o stringstore.o
	$(CC) $(CFLAGS) -o $@ $^

stringstore.o: stringstore.c stringstore.h
	$(CC) $(CFLAGS) -c -o $@ $<

stringstore-test.o: stringstore-test.c stringstore.h
	$(CC) $(CFLAGS) -c -o $@ $<


mapping-test: mapping-test.o mapping.o stringstore.o
	$(CC) $(CFLAGS) -o $@ $^

mapping-test.o: mapping-test.c mapping.h stringstore.h
	$(CC) $(CFLAGS) -c -o $@ $<

mapping.o: mapping.c mapping.h stringstore.h
	$(CC) $(CFLAGS) -c -o $@ $<

lines-test: lines-test.o lines.o mapping.o stringstore.o
	$(CC) $(CFLAGS) -o $@ $^

lines-test.o: lines-test.c lines.h mapping.h stringstore.h
	$(CC) $(CFLAGS) -c -o $@ $<

lines.o: lines.c lines.h mapping.h stringstore.h
	$(CC) $(CFLAGS) -c -o $@ $<




.PHONY: clean

clean:
	rm -f dis makedb printdb fakedis stringstore-test mapping-test lines-test
	rm -f *.o
	rm -f *.exe
