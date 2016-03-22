# The main makefile that builds up the amazing project

CC=gcc
CFLAGS= -Wall -pedantic -std=c11 -g -Wcpp

OBJDIR= ./src/
EXECDIR= ./bin/
TESTDIR= ./test/

all: $(EXECDIR)Startup $(EXECDIR)avatar 

$(EXECDIR)Startup: $(OBJDIR)AMStartup.o $(OBJDIR)amazing.h
	$(CC) $(CFLAGS) -o $@ $(OBJDIR)AMStartup.o

$(OBJDIR)AMStartup.o: $(OBJDIR)AMStartup.c $(OBJDIR)amazing.h
	$(CC) $(CFLAGS) -o $@ -c $(OBJDIR)AMStartup.c 

$(EXECDIR)avatar: $(OBJDIR)avatar.o $(OBJDIR)amazing.h
	$(CC) $(CFLAGS) -o $@ $(OBJDIR)avatar.o -pthread -lncurses

$(OBJDIR)avatar.o: $(OBJDIR)avatar.c $(OBJDIR)avatar.h $(OBJDIR)amazing.h
	$(CC) $(CFLAGS) -o $@ -c $(OBJDIR)avatar.c -pthread -lncurses

test: $(TESTDIR)unit_test

$(TESTDIR)unit_test: $(OBJDIR)unit_test.o 
	$(CC) $(CFLAGS) -o $@ $(OBJDIR)unit_test.o 

$(OBJDIR)unit_test.o: $(OBJDIR)unit_test.c 
	$(CC) $(CFLAGS) -o $@ -c $(OBJDIR)unit_test.c


clean:
	rm -f $(OBJDIR)~
	rm -f $(OBJDIR)#
	rm -f $(OBJDIR)*.o
	rm -f $(EXECDIR)*
	rm -f $(TESTDIR)*
