CC = gcc
FLAGS = -Wall -g
LIBS = -lpthread
TARGET = cache-proxy
COMMON = http-parse.o



all: $(TARGET)

$(TARGET): %: %.c $(COMMON)
	$(CC) $(FLAGS) -o $@ $< $(COMMON) $(LIBS) -L./picohttpparser -lpicohttpparser

clean:
	rm $(TARGET) *.o

%.o: %.c 
	$(CC) $(FLAGS) -c $<
