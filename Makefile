BINARY=driver.app
CC = gcc
OPT=-O0
DEPFLAGS=-MP -MD
LIBRARIES=""
CFLAGS=-Wall -g $(OPT) $(DEPFLAGS) 
CFILES = $(wildcard *.c)
OBJECTS=$(patsubst %.c,%.o,$(CFILES))
DEPFILES=$(patsubst %.c,%.d,$(CFILES))

all: $(BINARY) 

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^ $(LIBRARIES)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(LIBRARIES)

package:
	@tar -czvf collection.tar.gz *.[ch] .git .gitignore Makefile README.md

clean:
	rm -f $(BINARY) $(OBJECTS) $(DEPFILES)
check-mem:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all -s ./collection.run

-include $(DEPFILES)