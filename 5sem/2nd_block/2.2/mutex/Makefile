TARGET = queue-threads-mutex
SRCS = queue-mutex.c queue-threads-mutex.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET}

${TARGET}: queue-mutex.h ${SRCS}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS} ${LIBS} -o ${TARGET}


clean:
	${RM} -f *.o ${TARGET}
