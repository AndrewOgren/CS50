#Filename: Makefile
#Decription: The make file is to build up the query engine.

CC=gcc
CFLAGS= -Wall -pedantic -std=c11 -g

OBJDIR= ./src/
EXECDIR= ./bin/
TESTDIR= ./test/
UTILDIR= ./util/
UTILFLAG=-ltseutil
UTILLIB=$(UTILDIR)libtseutil.a
UTILC=$(UTILDIR)libtseutil.c
UTILH=$(UTILC:.c=.h)

OBJECTS= $(OBJDIR)query.o $(OBJDIR)rank.o
SOURCES= $(OBJDIR)query.c $(OBJDIR)rank.c $(OBJDIR)rank.h
CFILES= $(OBJDIR)query.c $(OBJDIR)rank.c

$(EXECDIR)query: $(UTILDIR)libtseutil.h $(OBJECTS) $(UTILLIB)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -L$(UTILDIR) $(UTILFLAG)

$(OBJDIR)query.o: $(UTILDIR)libtseutil.h $(SOURCES) $(UTILLIB)
	$(CC) $(CFLAGS) -o $@ -c $(OBJDIR)query.c -L$(UTILDIR) $(UTILFLAG)

$(OBJDIR)rank.o: $(UTILDIR)libtseutil.h $(OBJDIR)rank.c $(OBJDIR)rank.h $(UTILLIB)
	$(CC) $(CFLAGS) -o $@ -c $(OBJDIR)rank.c -L$(UTILDIR) $(UTILFLAG)

$(UTILLIB): $(UTILC) $(UTILH)
	cd $(UTILDIR); make;

testing: $(TESTDIR)queryengine_test.c
	cd $(TESTDIR); make;

clean:
	rm -f $(OBJDIR)~
	rm -f $(OBJDIR)#
	rm -f $(OBJDIR)*.o
	rm -f $(EXECDIR)* 
	rm -f $(UTILDIR)*.o
	rm -f $(UTILDIR)libtseutil.a
	rm -f $(TESTDIR)*.o
	rm -f $(TESTDIR)queryengine_test

