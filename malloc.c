#include "block.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define SIZEUP(sz) (((sz) + MEMSZ - 1) & ~(MEMSZ - 1))

struct block *first_block = NULL;

// first-fit algorithm
struct block *first_fit(struct block **last, size_t size) {
	struct block *b = first_block;
	while (b != NULL && !(b->size >= size && b->is_free)) {
		*last = b;
		b = b->next;
	}
	return b; // if b is NULL, that means there's no big enough block
}

// best-fit algorithm
struct block *best_fit(struct block **last, size_t size) {
	struct block *b = first_block;
	struct block *best = NULL;
	size_t min_size;
	while (b != NULL) {
		if (b->is_free && b->size >= size) {
			best = b;
			break;
		}
		*last = b;
		b = b->next;
	}
	while (b != NULL) {
		if (b->size < best->size && b->size >= size && b->is_free == 1) {
			best = b;
		}
		*last = b;
		b = b->next;
	}
	return best;
}

// there's no big enough block
struct block *extend_heap(struct block *last, size_t size) {
	struct block *b;
	b = (struct block *)sbrk(0);
	if (sbrk(BLOCK_SIZE + size) == (void *)-1)
		return NULL;
	b->size = size;
	b->is_free = 0;
	b->next = NULL;
	b->ptr = b->data; // set magic pointer to pointer data
	if (last) {
		last->next = b;
		b->prev = last;
	}
	return b;
}

void split_block(struct block *b, size_t s) {
	struct block *new;
	new = (struct block *)(b->data + s);
	new->size = b->size - s - BLOCK_SIZE ;
	new->next = b->next;
	new->is_free = 1;
	b->size = s;
	b->next = new;
}

/*
size_t align8(size_t size) {
	if (size & 0x7 == 0)
		return size;
	return ((size >> 3) + 1) << 3;
}
*/

struct block *(*find_funcs[])(struct block **, size_t) = {
	[0]	first_fit,
	[1]	best_fit,
};

void *my_malloc(int mode, size_t size) {
	if (mode != 0)
		mode = 1;

	struct block *b, *last;
	size_t s;
	/* 对齐地址 */
	s = SIZEUP(size);
	// printf("%ld\n", s);
	// s = align8(size);
	if(first_block) {
		/* 查找合适的block */
		last = first_block;
		b = find_funcs[mode](&last, s);
		if(b) {
			/* 如果可以，则分裂 */
			if ((b->size - s) >= ( BLOCK_SIZE + 8))
				split_block(b, s);
			b->is_free = 0;
		} else {
			/* 没有合适的block，开辟一个新的 */
			b = extend_heap(last, s);
			if(!b)
				return NULL;
		}
	} else {
		b = extend_heap(NULL, s);
		if(!b)
			return NULL;
		first_block = b;
		first_block->prev = NULL;
	}
	return b->data;
}

void *my_calloc(int mode, size_t nmemb, size_t size) {
	if (mode != 0)
		mode = 1;
	void *p = my_malloc(mode, nmemb * size);
	size_t s = SIZEUP(nmemb * size);
	memset(p, 0, s);
	return p;
}

/*
void merge(struct block *b) {
	// forward
	struct block *ptr = b;
	b = b->next;
	while (b != NULL) {
		if (b->is_free) {
			ptr->size += (BLOCK_SIZE + b->size);
			ptr->next = b->next;
			if (b->next)
				b->next->prev = ptr;
		}
		else 
			break;
		b = b->next;
	}

	// backward
	b = ptr->prev;
	while (b != NULL) {
		if (b->is_free) {
			b->size += (BLOCK_SIZE + ptr->size);
			b->next = ptr->next;
			if (ptr->next)
				ptr->next->prev = b;
		}
		else
			break;
		ptr = b;
		b = b->prev;
	}
}
*/

struct block *merge(struct block *b) {
	if (b->next->is_free)	{
		b->size += (BLOCK_SIZE + b->next->size);
		b->next = b->next->next;
		if (b->next)
			b->next->prev = b;
	}
	return b;
}

int my_free(void *ptr) {
	if (first_block == NULL) {
		// fprintf(stderr, "aaa\n");
		goto bad;
	}

	if (ptr < (void *)first_block->data || ptr > sbrk(0)){
		// fprintf(stderr, "bbb\n");
		goto bad;
	}

	struct block *meta = (struct block *)(ptr - BLOCK_SIZE);
	if (meta->is_free && meta->ptr != meta->data) {
		// fprintf(stderr, "ccc\n");
		goto bad;
	}

	meta->is_free = 1;
	if (meta->prev && meta->prev->is_free)
		meta = merge(meta->prev);
	if (meta->next)
		meta = merge(meta);
	else {
		if (meta->prev == NULL) // the last block
			first_block = NULL;	
		else 
			meta->prev->next = NULL;
		brk(meta);
	}
	return 1;

	bad:
		// fprintf(stderr, "my_free failed\n");
		return 0;
}
