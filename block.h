#ifndef _BLOCK_H
#define _BLOCK_H
#include <sys/types.h>
#define MEMSZ 8
#define BLOCK_SIZE 40
// #define MGNUM 0X3F
// typedef unsigned char uint8;
// struct block;
struct block {
	size_t size;
	struct block *next;
	struct block *prev;
	int is_free;
	int padding; // 使sizeof(struct block)为40(8的倍数)，用于字节对齐
	void *ptr;
	char data[1]; // 这是一个虚拟字段，表示数据区的第一个字节
};
#endif
