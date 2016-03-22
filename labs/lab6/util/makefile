#Filename: makefile
#Description: The make file is used to build the library for tse

CC=gcc
CFLAGS= -Wall

libtseutil.a: libtseutil.o
	ar -cvq libtseutil.a libtseutil.o

libtseutil.o: libtseutil.c libtseutil.h
	$(CC) $(CFLAGS) -c libtseutil.c