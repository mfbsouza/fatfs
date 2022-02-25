#ifndef __FATFS_H__
#define __FATFS_H__

#include <stdio.h>
#include <stdint.h>

#define MAX_MBR_PARTITIONS 4

/* Structs */

struct fatfs_t {
	FILE* disk;
	struct part_table* pt;
	uint8_t types[MAX_MBR_PARTITIONS];
	struct bpb_table* bpb[MAX_MBR_PARTITIONS];
	void* ebr[MAX_MBR_PARTITIONS];
};

struct part_table;
struct bpb_table;

/* Functions */

int fatfs_init(struct fatfs_t*, const char*);
void fatfs_exit(struct fatfs_t*);

void fatfs_print_info(struct fatfs_t*, FILE*);

#endif /* __FATFS_H__ */
