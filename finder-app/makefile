CROSS_COMPILE ?=
CC ?= $(CROSS_COMPILE)gcc

all:
	writer
writer: writer.o
	${CC} ${LDFLAGS} writer.o -o writer
writer.o: writer.c
	${CC} ${CFLAGS} ${INCLUDES} -c writer.c -o writer.o
clean:
	rm -f writer.o writer