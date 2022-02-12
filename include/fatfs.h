#ifndef __FATFS_H__
#define __FATFS_H__

#include <stdio.h>
#include <stdint.h>

struct partition_table_entry {
	uint8_t drive_attr;
	uint8_t start_chs_addr[3];
	uint8_t type;
	uint8_t end_chs_addr[3];
	uint32_t start_lba_addr;
	uint32_t sectors_cnt;
} __attribute__((packed));

struct mbr_sector {
	uint8_t boot_code[446];
	struct partition_table_entry pt_entry[4];
	uint8_t signature[2];
} __attribute__((packed));

size_t read_boot_sector(struct mbr_sector*, FILE*);

#endif /* __FATFS_H__ */
