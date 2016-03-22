CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -g -lcurl

crawler: crawler.o web.o list.o hashtable.o
	$(CC) $(CFLAGS) -o crawler web.o list.o hashtable.o crawler.o

crawler.o: crawler.c web.h list.h hashtable.h common.h utils.h common.h
	$(CC) $(CFLAGS) -c crawler.c

web.o: web.c web.h common.h
	$(CC) $(CFLAGS) -c web.c

list.o: list.c list.h common.h hashtable.h utils.h
	$(CC) $(CFLAGS) -c list.c

hashtable.o: hashtable.c hashtable.h common.h
	$(CC) $(CFLAGS) -c hashtable.c

clean:
	rm -f *~
	rm -f *#
	rm -f *.o
	rm -f ./crawler
	rm -f ./*~
	rm -f ./*#
	rm -f ./*.o