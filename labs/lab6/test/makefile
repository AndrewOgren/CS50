#filename: makefile
#Description: this makefile is used to build the testing file
CC= gcc
CFLAGS= -Wall -pedantic -std=c11 -g

UTILDIR= ../util/
UTILFLAG= -ltseutil
UTILLIB=$(UTILDIR)libtseutil.a
OBJDIR= ../src/



queryengine_test: $(UTILDIR)libtseutil.h queryengine_test.o $(UTILLIB)
	$(CC) $(CFLAGS) -o $@ queryengine_test.o $(OBJDIR)rank.o -L$(UTILDIR) $(UTILFLAG)

queryengine_test.o: $(UTILDIR)libtseutil.h $(OBJDIR)rank.c $(OBJDIR)rank.h $(OBJDIR)query.c $(UTILLIB)
	$(CC) $(CFLAGS) -o $@ -c queryengine_test.c -L$(UTILDIR) $(UTILFLAG)