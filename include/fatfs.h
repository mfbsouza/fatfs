#ifndef __FATFS_H__
#define __FATFS_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_MBR_PARTITIONS 4
#define SECTOR_SIZE 512
#define SIG_OFFSET 0x1FE
#define PT_OFFSET 0x1BE
#define EBR_OFFSET 0x024

/* Sectors Headers */

// Partition Table Entry

struct partition_table {
	uint8_t drive_attr;
	uint8_t start_chs_addr[3];
	uint8_t type;
	uint8_t end_chs_addr[3];
	uint32_t start_lba_addr;
	uint32_t sectors_cnt;
} __attribute__((packed));

// MBR Sector Header

struct mbr_sector {
	uint8_t boot_code[446];
	struct partition_table pt[4];
	uint8_t signature[2];
} __attribute__((packed));

// BPB (BIOS Parameter Block)

struct bpb_table {
	uint8_t jmp_code[3];
	char oem_id[8];
	uint16_t sector_size;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t fat_cnt;
	uint16_t root_entry_cnt;
	uint16_t sector_cnt; // if it's zero later field is used
	uint8_t media_descriptor;
	uint16_t sectors_per_fat;
	uint16_t sectors_per_track;
	uint16_t head_cnt;
	uint32_t hidden_sectors_cnt;
	uint32_t large_sector_cnt;
} __attribute__((packed));

// Extended Boot Record for FAT12 and FAT16

struct fat_ebr {
	uint8_t drive_number;
	uint8_t flags;
	uint8_t signature; // must be 0x28 or 0x29.
	uint32_t volume_id;
	char volume_label[11];
	char fs_type[8];
} __attribute__((packed));

// Extended Boot Record for FAT32

struct fat32_ebr {
	uint32_t sectors_per_fat;
	uint16_t flags;
	uint16_t fat_version;
	uint32_t root_cluster_number; // often set to 2.
	uint16_t fsinfo_sector_number;
	uint16_t backup_sector_number;
	uint8_t reserved[12]; // when formated should be zero.
	struct fat_ebr ebr;
} __attribute__((packed));

// FAT12or16 Volume Boot Record Sector Header

struct fat_vbr_sector {
	struct bpb_table bpb;
	struct fat_ebr ebr;
	uint8_t boot_code[448];
	uint8_t signature[2];
} __attribute__((packed));

// FAT32 Volume Boot Record Sector Header

struct fat32_vbr_sector {
	struct bpb_table bpb;
	struct fat32_ebr ebr;
	uint8_t boot_code[420];
	uint8_t signature[2];
} __attribute__((packed));

/* Functions */

int validate_mbr(FILE*);
void* load_sector(const int, const int, FILE*);
struct partition_table* load_part_table(FILE*);
void print_pt_info(FILE*, struct partition_table*);
void print_vbr_info(FILE*, void*, const uint8_t);

#endif /* __FATFS_H__ */
