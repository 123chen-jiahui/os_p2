all: main test
test: test.o malloc.o
	gcc test.o malloc.o -o test
main: main.o malloc.o
	gcc main.o malloc.o -o main
test.o: lib.h block.h test.c
	gcc -c test.c
main.o: lib.h main.c
	gcc -c main.c
malloc.o: block.h malloc.c
	gcc -c malloc.c

clean:
	@echo "cleaning project"
	-rm *.o main test
	@echo "clean completed"

.PHONY: clean
