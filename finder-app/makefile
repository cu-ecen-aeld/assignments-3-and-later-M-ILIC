all: writer
writer: writer.o
	${CC} ${LDFLAGS} writer.o -o writer
writer.o: writer.c
	${CC} ${CFLAGS} ${INCLUDES} -c writer.c -o writer.o

.PHONY: clean
clean:
	rm -f writer.o writer