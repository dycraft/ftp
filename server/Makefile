CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lm
SOURCES=main.c server.c fdlist.c command.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ftpserver

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ -lpthread

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ -lpthread

clean:
	rm -rf *.o ftpserver
