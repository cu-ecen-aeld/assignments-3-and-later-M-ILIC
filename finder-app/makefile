all:
	writer
writer: writer.o
	${CC}gcc writer.o -o writer
writer.o: writer.c
	${CC}gcc -c writer.c -o writer.o
clean:
	rm -f writer.o writer