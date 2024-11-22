TARGET = queue-threads-sem
SRCS = queue-sem.c queue-threads-sem.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET}

${TARGET}: queue-sem.h ${SRCS}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS} ${LIBS} -o ${TARGET}


clean:
	${RM} -f *.o ${TARGET}
