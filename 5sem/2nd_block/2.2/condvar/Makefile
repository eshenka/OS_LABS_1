TARGET = queue-threads-condvar
SRCS = queue-condvar.c queue-threads-condvar.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET}

${TARGET}: queue-condvar.h ${SRCS}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS} ${LIBS} -o ${TARGET}


clean:
	${RM} -f *.o ${TARGET}
