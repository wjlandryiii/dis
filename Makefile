CC=gcc
CFLAGS=-lcapstone -g

all:	dis \
	makedb \
	printdb \
	fakedis \
	stringstore-test \
	mapping-test \
	lines-test

bytes.o: 	bytes.c bytes.h
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
