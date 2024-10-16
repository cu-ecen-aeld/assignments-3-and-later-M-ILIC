all: client
client: client.o
client.o: client.c
	${CC} ${CFLAGS} ${INCLUDES} -c client.c -o client.o -Wall
.PHONY: clean
clean: 
	rm -f client.o client