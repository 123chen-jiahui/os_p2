#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "block.h"
#include "lib.h"

extern struct block *first_block;
const char COLOR_GREEN[] = "\033[0;32m";
const char COLOR_RESET[] = "\033[0m";
// const char COLOR_RED[] = "\033[0;31m";
const char str[2][10] = {"first-fit", "best-fit"};

struct big_staff {
	char c[1000];
	int i[1000];
	double d[1000];
};

int assert_equals_int(char *msg, int expected, int actual) {
	if (expected != actual) {
		printf("Assertion error: expected %s to be %d but got %d\n", msg, expected, actual);
		return 0;
	} else {
		return 1;
	}
}

int show_info() {
	printf("\ninfo start:\n");
	int count = 0;
	struct block *b = first_block;
	if (b == NULL) {
		printf("info: heap is empty\n");
		printf("info end\n");
		return count;
	}
	while (b != NULL) {
		if (b->is_free) {
			printf("info: the %dth block is free and the size is %ld\n", ++count, b->size);
		} else {
			printf("info: the %dth block is in use and the size is %ld\n", ++count, b->size);
		}
		b = b->next;
	}
	printf("info end\n");
	return count;
}

int test_and_print(char *label, int (*run_test)()) {
	printf("\nTesting %s...\n", label);
	int result = run_test();
	if (result) {
		printf("%sAll %s tests passed!%s\n", COLOR_GREEN, label, COLOR_RESET);
	} else {
		printf("Not all %s tests passed.\n", label);
	}
	return result;
}

int test_simple_malloc_1(int mode) {
	int *tmp = (int *)my_malloc(mode, sizeof(int));
	if (tmp == NULL)
		return 0;
	if (!assert_equals_int("block_num", 1, show_info()))
		return 0;
	*tmp = 1;
	if (!assert_equals_int("free_result", 1, my_free(tmp)))
		return 0;
	if (!assert_equals_int("block_num", 0, show_info()))
		return 0;
	return 1;
}

int test_simple_malloc_2(int mode) {
	double *tmp = my_malloc(mode, sizeof(double));
	if (tmp == NULL)
		return 0;
	if (!assert_equals_int("block_num", 1, show_info()))
		return 0;
	*tmp = 6.081;
	if (!assert_equals_int("free_result", 1, my_free(tmp)))
		return 0;
	if (!assert_equals_int("block_num", 0, show_info()))
		return 0;
	return 1;
}

int test_simple_malloc_3(int mode) {
	struct big_staff *tmp = my_malloc(mode, sizeof(struct big_staff));
	if (tmp == NULL)
		return 0;
	memset(tmp, 0, sizeof(struct big_staff));
	if (!assert_equals_int("block_num", 1, show_info()))
		return 0;
	if (!assert_equals_int("free_result", 1, my_free(tmp)))
		return 0;
	if (!assert_equals_int("block_num", 0, show_info()))
		return 0;
	return 1;
}

int test_simple_malloc() {
	for (int i = 0; i < 2; i ++) {
		if (!test_simple_malloc_1(i)) {
			printf("test_simple_malloc_1 failed(%s).\n", str[i]);
			return 0;
		}

		if (!test_simple_malloc_2(i)) {
			printf("test_simple_malloc_2 failed(%s).\n", str[i]);
			return 0;
		}

		if (!test_simple_malloc_3(i)) {
			printf("test_simple_malloc_3 failed(%s).\n", str[i]);
			return 0;
		}
	}
	return 1;
}

int test_many_malloc_1(int mode) {
	int *a[10];
	for (int i = 0; i < 10; i ++) {
		a[i] = (int *)my_malloc(mode, sizeof(int) * 10);
		if (a[i] == NULL)
			return 0;
	}
	if (!assert_equals_int("block_num", 10, show_info()))	
		return 0;
	for (int i = 0; i < 10; i ++) {
		if (!assert_equals_int("free_result", 1, my_free(a[i])))
			return 0;
	}
	if (!assert_equals_int("block_num", 0, show_info()))
		return 0;
	return 1;
}

int test_many_malloc_2(int mode) {
	int **a;
	a = (int **)my_malloc(mode, sizeof(int *) * 100);
	if (a == NULL)
		return 0;
	if (!assert_equals_int("block_num", 1, show_info()))
		return 0;
	for (int i = 0; i < 100; i ++) {
		a[i] = (int *)my_malloc(mode, sizeof(int) * 100);
		if (a[i] == NULL)
			return 0;
	}
	if (!assert_equals_int("block_num", 101, show_info()))
		return 0;

	for (int i = 0; i < 50; i ++) {
		if (!assert_equals_int("free_result", 1, my_free(a[i])))
			return 0;
	}	
	if (!assert_equals_int("block_num", 52, show_info()))
		return 0;
	
	for (int i = 50; i < 100; i ++) {
		if (!assert_equals_int("free_result", 1, my_free(a[i])))
			return 0;
	}

	if (!assert_equals_int("block_num", 1, show_info()))
		return 0;
	
	if (!assert_equals_int("free_result", 1, my_free(a)))
			return 0;
	if (!assert_equals_int("block_num", 0, show_info()))
		return 0;

	return 1;
}

int test_many_malloc() {
	for (int i = 0; i < 2; i ++) {
		if (!test_many_malloc_1(i)) {
			printf("test_many_malloc_1 failed(%s).\n", str[i]);
			return 0;
		}
		if (!test_many_malloc_2(i)) {
			printf("test_many_malloc_2 failed(%s).\n", str[i]);
			return 0;
		}
	}	
}

int test_jump_free_1(int mode) {
	int *a[100];
	for (int i = 0; i < 100; i ++) {
		a[i] = (int *)my_malloc(mode, sizeof(int) * 10);
		if (a[i] == NULL)
			return 0;
	}
	if (!assert_equals_int("block_num", 100, show_info()))
		return 0;

	for (int i = 0; i < 100; i += 10) {
		if (!assert_equals_int("free_result", 1, my_free(a[i])))
			return 0;
		if (!assert_equals_int("free_result", 1, my_free(a[i + 1])))
			return 0;
	}
	if (!assert_equals_int("block_num", 100 - 10, show_info()))
		return 0;

	for (int i = 0; i < 100; i ++) {
		if (i % 10 == 0)
			i ++;
		else {
			if (!assert_equals_int("free_result", 1, my_free(a[i])))
				return 0;
		}
	}
	if (!assert_equals_int("block_num", 0, show_info()))
		return 0;
	return 1;
}

int test_jump_free_2(int mode) {
	srand((int)time(0));
	int *a[100];
	int index[100];
	memset(index, 0, sizeof(index));
	for (int i = 0; i < 100; i ++)
		a[i] = (int *)my_malloc(mode, sizeof(int) * 10);

	for (int i = 0; i < 30; i ++) {
		int t = rand() % 100;
		if (index[t] == 0) {
			index[t] = 1;
			if (!assert_equals_int("free_result", 1, my_free(a[t])))
				return 0;
		}	
	}

	int num = 100;
	int flag = 0;
	for (int i = 0; i < 100; i ++) {
		if (flag && index[i])	
			num --;
		if (index[i])
			flag = 1;
		else
			flag = 0;	
	}
	if (index[99] == 1)
		num --;
	if (!assert_equals_int("block_num", num, show_info()))
		return 0;

	for (int i = 0; i < 100; i ++) {
		if (index[i] == 0)
			if (!assert_equals_int("free_result", 1, my_free(a[i])))
				return 0;
	}
	return 1;
}

int test_jump_free() {
	for (int i = 0; i < 2; i ++) {
		if (!test_jump_free_1(i)) {
			printf("test_jump_free_1 failed(%s).\n", str[i]);
			return 0;
		}
		if (!test_jump_free_2(i)) {
			printf("test_jump_free_2 failed(%s).\n", str[i]);
			return 0;
		}
	}
}

// free memory hasn't been allocated
int test_invalid_free_1() {
	int *a[100];
	for (int i = 0; i < 100; i ++)
		if (!assert_equals_int("free_result", 0, my_free(a[i])))
			return 0;
	return 1;
}

// double free
int test_invalid_free_2(int mode) {
	int *a[100];
	for (int i = 0; i < 100; i ++) {
		a[i] = (int *)my_malloc(mode, sizeof(int) * 10);
		my_free(a[i]);
		if (!assert_equals_int("free_result", 0, my_free(a[i])))
			return 0;
	}
	return 1;
}

int test_invalid_free() {
	if (!test_invalid_free_1()) {
		printf("test_invalid_free_1 failed.\n");
		return 0;
	}
	for (int i = 0; i < 2; i ++) {
		if (!test_invalid_free_2(i)) {
			printf("test_invalid_free_2 failed(%s).\n", str[i]);
		}
	}	
	return 1;
}

int test_calloc() {
	for (int i = 0; i < 2; i ++) {
		int *a;
		a = (int *)my_calloc(i, 10, sizeof(int));
		for (int i = 0; i < 10; i ++)
			if (!assert_equals_int("value in memory", 0, a[i]))
				return 0;
		my_free(a);
	}
	return 1;
}

int main() {
	printf("Testing start...\n");
	if (!test_and_print("test_simple_malloc", test_simple_malloc)) {
		return 0;
	}
	if (!test_and_print("test_many_malloc", test_many_malloc)) {
		return 0;
	}
	if (!test_and_print("test_jump_free", test_jump_free)) {
		return 0;
	}
	if (!test_and_print("test_invalid_free", test_invalid_free)) {
		return 0;
	}
	if (!test_and_print("test_calloc", test_calloc)) {
		return 0;
	}
	printf("\n%sAll tests passed!%s\n", COLOR_GREEN, COLOR_RESET);
	return 0;
}
