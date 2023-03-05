SOURCES=main.c

OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=./server

all: build
	$(EXECUTABLE)

build: $(OBJECTS)
	gcc -o $(EXECUTABLE) $^

.c.o:
	gcc -c -o $@ -Wall -Wextra $<

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)