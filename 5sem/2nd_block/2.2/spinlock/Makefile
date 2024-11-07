TARGET = queue-threads-spinlock
SRCS = queue-spinlock.c queue-threads-spinlock.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET}

${TARGET}: queue-spinlock.h ${SRCS}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS} ${LIBS} -o ${TARGET}


clean:
	${RM} -f *.o ${TARGET}
