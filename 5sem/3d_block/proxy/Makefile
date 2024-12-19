CC = gcc
FLAGS = -Wall -g
LIBS = -lpthread
TARGET = cache-proxy
COMMON = http-parse.o network.o list.o cache.o



all: $(TARGET)

$(TARGET): %: %.c $(COMMON)
	$(CC) $(FLAGS) -o $@ $< $(COMMON) $(LIBS) -L./picohttpparser -lpicohttpparser -L./hashmap.c -lhashmap

clean:
	rm $(TARGET) *.o

%.o: %.c 
	$(CC) $(FLAGS) -c $<
