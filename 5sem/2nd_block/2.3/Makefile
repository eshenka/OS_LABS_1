CC=gcc
FLAGS=-Wall -g 
LIBS=-lpthread
TARGET_DIR=target

TARGETS=mutex spin rwlock
COMMON=main.o linked_list.o

all: $(TARGETS)
	rm *.o

%.o: %.c
	$(CC) $(FLAGS) -c $<

$(TARGETS): %: list_%.o $(COMMON)
	$(CC) $(FLAGS) -o $@ $< $(COMMON) $(LIBS)


clean:
	rm $(TARGETS)
