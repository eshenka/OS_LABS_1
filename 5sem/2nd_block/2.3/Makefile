CC=gcc
FLAGS=-Wall -g 
LIBS=-lpthread

TARGETS=mutex spin rwlock 
MYTARGETS=myspin mymutex
COMMON=main.o linked_list.o

all: $(TARGETS) $(MYTARGETS)
	rm *.o

%.o: %.c
	$(CC) $(FLAGS) -c $<

$(TARGETS): %: list_%.o $(COMMON)
	$(CC) $(FLAGS) -o $@ $< $(COMMON) $(LIBS)

$(MYTARGETS): %: list_%.o %l.o $(COMMON)
	$(CC) $(FLAGS) -o $@ $< $@l.o $(COMMON) $(LIBS)


clean:
	rm $(TARGETS) $(MYTARGETS)
