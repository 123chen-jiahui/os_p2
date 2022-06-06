#include <stdio.h>
#include "lib.h"
#include "block.h"

int main() {
	int b;
	int *a = my_malloc(1, sizeof(int));
	*a = 4;
	printf("%d\n", *a);
	printf("%p %p\n", a, &b);
	my_free((void *)a);
	my_free((void *)0);
	return 0;
}
