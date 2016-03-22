CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb

OBJDIR = src
EXECDIR = bin

OBJECTS = $(OBJDIR)/indexer.o $(OBJDIR)/file.o $(OBJDIR)/web.o $(OBJDIR)/hashtable.o


$(EXECDIR)/indexer: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECDIR)/indexer $(OBJECTS)

$(OBJDIR)/indexer.o: $(OBJDIR)/indexer.c $(OBJDIR)/file.h $(OBJDIR)/web.h $(OBJDIR)/hashtable.o
	$(CC) $(CFLAGS) -o $(OBJDIR)/indexer.o  -c $(OBJDIR)/indexer.c

$(OBJDIR)/web.o: $(OBJDIR)/web.c $(OBJDIR)/web.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/web.o -c $(OBJDIR)/web.c

$(OBJDIR)/file.o: $(OBJDIR)/file.c $(OBJDIR)/file.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/file.o -c $(OBJDIR)/file.c

$(OBJDIR)/hashtable.o: $(OBJDIR)/hashtable.c $(OBJDIR)/hashtable.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/hashtable.o -c $(OBJDIR)/hashtable.c

clean:
	rm -f $(OBJDIR)/~
	rm -f $(OBJDIR)/#
	rm -f $(OBJDIR)/*.o
	rm -f $(EXECDIR)/*



	
