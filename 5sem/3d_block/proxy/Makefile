CC = gcc
FLAGS = -Wall -g
LIBS = -lpthread
TARGET = cache-proxy
COMMON = http-parse.o network.o list.o cache.o picohttpparser/picohttpparser.o hashmap.c/hashmap.o


all: $(TARGET)

$(TARGET): %: %.c $(COMMON)
	$(CC) $(FLAGS) -o $@ $< $(COMMON) $(LIBS) 

clean:
	rm $(TARGET) *.o

%.o: %.c 
	$(CC) $(FLAGS) -c $<
