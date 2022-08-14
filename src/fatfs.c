#include "fatfs.h"
#include <stdlib.h>
#include <string.h>

#define SECTOR_SIZE 		512
#define SIG_OFFSET 		0x1FE
#define PT_OFFSET 		0x1BE
#define EBR_OFFSET 		0x024

/* partition table entry */
struct part_table {
	uint8_t drive_attr;
	uint8_t start_chs_addr[3];
	uint8_t type;
	uint8_t end_chs_addr[3];
	uint32_t start_lba_addr;
	uint32_t sectors_cnt;
} __attribute__((packed));

/* bios parameter block */
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
	uint16_t sectors_per_fat; // fat12 or fat16 only
	uint16_t sectors_per_track;
	uint16_t head_cnt;
	uint32_t hidden_sectors_cnt;
	uint32_t large_sector_cnt;
} __attribute__((packed));

/* extended boot record for fat12 and fat16 */
struct fat12_ebr {
	uint8_t drive_number;
	uint8_t nt_flags;
	uint8_t signature; // must be 0x28 or 0x29.
	uint32_t volume_id;
	char volume_label[11];
	char fs_type[8];
} __attribute__((packed));

/* extended boot record for fat32 */
struct fat32_ebr {
	uint32_t sectors_per_fat;
	uint16_t flags;
	uint16_t fat_version;
	uint32_t root_cluster_number; // often set to 2.
	uint16_t fsinfo_sector_number;
	uint16_t backup_sector_number;
	uint8_t reserved[12]; // when formated should be zero.
	uint8_t drive_number;
	uint8_t nt_flags;
	uint8_t signature; // must be 0x28 or 0x29.
	uint32_t volume_id;
	char volume_label[11];
	char fs_type[8];
} __attribute__((packed));

/* Functions Implementation */

static void* load_data(size_t size, const int offset, const int count, FILE* disk)
{
	int red = 0;
	void* buf = NULL;

	// alloc memory for the disk read
	buf = malloc(size * count);

	if (!buf) {
		fprintf(stderr, "error allocating memory\n");
		return NULL;
	}

	// read disk
	fseek(disk, offset, SEEK_SET);
	red = fread(buf, size, count, disk);

	if (red <= 0) {
		fprintf(stderr, "error reading disk, red: %d\n", red);
		return NULL;
	}

	// return the pointer to the chunk in memory
	return buf;
}

static inline void* load_part_table(FILE* disk)
{
	return load_data(sizeof(struct part_table), PT_OFFSET, 4, disk);
}

static inline void* load_sector(const int offset, const int count, FILE* disk)
{
	return load_data(SECTOR_SIZE, offset, count, disk);
}

static inline int validate_mbr(FILE* disk)
{
	uint8_t boot_sig[] = {0x55, 0xAA};
	uint8_t* sig = (uint8_t*) load_data(sizeof(uint8_t), SIG_OFFSET, 2, disk);
	int ret = memcmp(boot_sig, sig, 2);
	free(sig);
	return ret;
}

int fatfs_init(struct fatfs_t* ptr, const char* path)
{
	// opening disk
	ptr->disk = NULL;
	ptr->disk = fopen(path, "rb");
	if (ptr->disk == NULL) {
		fprintf(stderr, "error opening disk");
		return -1;
	}

	// check if disk has a valid MBR sector
	if (validate_mbr(ptr->disk)) {
		fprintf(stderr, "disk has no valid mbr sector");
		return -2;
	}

	// load the partition tables to memory
	ptr->pt = load_part_table(ptr->disk);

	// load bpb tables and populate partition types array
	for (int i = 0; i < MAX_MBR_PARTITIONS; i++) {
		if (ptr->pt[i].type != 0) {
			ptr->bpb[i] = (struct bpb_table*) 
				load_data(
						sizeof(struct bpb_table),
						ptr->pt[i].start_lba_addr * SECTOR_SIZE,
						1,
						ptr->disk
					);
		}
		ptr->types[i] = ptr->pt[i].type;
	}

	// done
	return 0;
}

void fatfs_exit(struct fatfs_t* ptr)
{
	// undo everything done in the init function
	for (int i = 0; i < MAX_MBR_PARTITIONS; i++) {
		if (ptr->types[i] != 0) {
			free(ptr->bpb[i]);
			ptr->bpb[i] = NULL;
		}
		ptr->types[i] = 0;
	}
	free(ptr->pt);
	ptr->pt = NULL;
	fclose(ptr->disk);
	ptr->disk = NULL;
}

void fatfs_print_info(struct fatfs_t* ptr, FILE* out)
{
	int i;
	for (i = 0; i < MAX_MBR_PARTITIONS; i++) {
		if (ptr->types[i] != 0) {
			fprintf(out, "partition %d:\n\ttype: %02X\n", i, ptr->types[i]);
			fprintf(out, "\tpartition lba start:\t%d\n", ptr->pt[i].start_lba_addr);
			fprintf(out, "\tsize: %d sectors * 512bytes = %dMB\n", ptr->pt[i].sectors_cnt, (ptr->pt[i].sectors_cnt*512/1024)/1024);
			fprintf(out, "\tOEM String:\t\t%s\n", ptr->bpb[i]->oem_id);
			fprintf(out, "\treserved sectors:\t%d\n", ptr->bpb[i]->reserved_sectors);
			fprintf(out, "\tfat size in sectors:\t%d\n", ptr->bpb[i]->sectors_per_fat);
			fprintf(out, "\tsector count:\t\t%d\n", ptr->bpb[i]->sector_cnt);
			fprintf(out, "\tlarge sector count:\t%d\n", ptr->bpb[i]->large_sector_cnt);
		}
	}
}
