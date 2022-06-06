all: test lib.a
test: test.o malloc.o
	gcc test.o malloc.o -o test
	./test
lib.a: malloc.o
	ar rcs lib.a malloc.o
test.o: lib.h block.h test.c
	gcc -c test.c
malloc.o: block.h malloc.c
	gcc -c malloc.c

clean:
	@echo "cleaning project"
	-rm *.o test lib.a
	@echo "clean completed"

.PHONY: clean
