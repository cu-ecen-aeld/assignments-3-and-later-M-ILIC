all: aesdsocket
aesdsocket: aesdsocket.o
aesdsocket.o: aesdsocket.c
	${CC} ${CFLAGS} ${INCLUDES} -c aesdsocket.c -o aesdsocket.o -Wall
.PHONY: clean
clean: 
	rm -f aesdsocket.o aesdsocket