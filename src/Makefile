
OUT = usimm
BINDIR = ../bin
OBJDIR = ../obj
OBJS = $(OBJDIR)/main.o $(OBJDIR)/memory_controller.o $(OBJDIR)/scheduler.o $(OBJDIR)/cache.o $(OBJDIR)/prefetcher.o $(OBJDIR)/plb.o
CC = gcc
DEBUG = -g
CFLAGS = -std=c99 -Wall -c $(DEBUG) 
LFLAGS = -Wall $(DEBUG)


$(BINDIR)/$(OUT): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(BINDIR)/$(OUT)	-lm 
	chmod 777 $(BINDIR)/$(OUT)

$(OBJDIR)/main.o: main.c processor.h configfile.h memory_controller.h scheduler.h params.h cache.h prefetcher.h plb.h
	$(CC) $(CFLAGS) main.c -o $(OBJDIR)/main.o -lm 
	chmod 777 $(OBJDIR)/main.o

$(OBJDIR)/memory_controller.o: memory_controller.c utlist.h utils.h params.h memory_controller.h scheduler.h processor.h cache.h prefetcher.h plb.h
	$(CC) $(CFLAGS) memory_controller.c -o $(OBJDIR)/memory_controller.o -lm 
	chmod 777 $(OBJDIR)/memory_controller.o

$(OBJDIR)/scheduler.o: scheduler.c scheduler.h utlist.h utils.h memory_controller.h params.h
	$(CC) $(CFLAGS) scheduler.c -o $(OBJDIR)/scheduler.o -lm 
	chmod 777 $(OBJDIR)/scheduler.o

$(OBJDIR)/cache.o: cache.c cache.h configfile.h params.h 
	$(CC) $(CFLAGS) cache.c -o $(OBJDIR)/cache.o -lm 
	chmod 777 $(OBJDIR)/cache.o

$(OBJDIR)/prefetcher.o: prefetcher.c prefetcher.h configfile.h params.h 
	$(CC) $(CFLAGS) prefetcher.c -o $(OBJDIR)/prefetcher.o -lm 
	chmod 777 $(OBJDIR)/prefetcher.o

$(OBJDIR)/plb.o: plb.c plb.h configfile.h params.h 
	$(CC) $(CFLAGS) plb.c -o $(OBJDIR)/plb.o -lm 
	chmod 777 $(OBJDIR)/plb.o	

clean:
	rm -f $(BINDIR)/$(OUT) $(OBJS)

