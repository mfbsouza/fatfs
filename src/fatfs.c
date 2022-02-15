#include "fatfs.h"

struct partition_table* load_part_table(FILE* disk)
{
	int red = 0;
	struct partition_table* pt = NULL;

	// alloc memory for the partition table
	pt = (struct partition_table*) malloc(sizeof(struct partition_table) * 4);

	if (!pt) {
		fprintf(stderr, "error allocating memory for the partiton table\n");
		exit(-1);
	}

	// read disk
	fseek(disk, PT_OFFSET, SEEK_SET);
	red = fread(pt, sizeof(struct partition_table), 4, disk);

	if (red <= 0) {
		fprintf(stderr, "error reading disk, red: %d\n", red);
		exit(-1);
	}

	// return the pointer to the patition tables in memory
	return pt;
}

int validate_mbr(FILE* disk)
{
	int ret, red = 0;
	uint8_t* signature = NULL;

	//alloc memory for the mbr signature
	signature = (uint8_t*) malloc(sizeof(uint8_t) * 2);

	if (!signature) {
		fprintf(stderr, "error allocating memory for the signature\n");
		exit(-1);
	}

	// read disk
	fseek(disk, SIG_OFFSET, SEEK_SET);
	red = fread(signature, sizeof(uint8_t), 2, disk);

	if (red <= 0) {
		fprintf(stderr, "error reading disk, red: %d\n", red);
		exit(-1);
	}

	// check signature
	if (signature[0] == 0x55 && signature[1] == 0xAA) {
		ret = 0;
	} else {
		ret = -1;
	}

	// free the mbr signature
	free(signature);

	return ret;
}

void* load_sector(const int offset, const int count, FILE* disk)
{
	int red = 0;
	void* buf = NULL;

	// alloc memory for the sector(s)
	buf = malloc(SECTOR_SIZE * count);

	if (!buf) {
		fprintf(stderr, "error allocating memory for the sector\n");
		exit(-1);
	}

	// read disk
	fseek(disk, offset, SEEK_SET);
	red = fread(buf, SECTOR_SIZE, count, disk);

	if (red <= 0) {
		fprintf(stderr, "error reading disk, red: %d\n", red);
		exit(-1);
	}

	// return the pointer to the sector(s) in memory
	return buf;
}

void print_pt_info(FILE* out, struct partition_table* pt)
{
	int i;
	for (i = 0; i < MAX_MBR_PARTITIONS; i++) {
		if (pt[i].type != 0) {
			fprintf(out, "parition %d:\n\ttype: %02X\n", i, pt[i].type);
			fprintf(out, "\tsize: %d sectors * 512 bytes = %dMB\n", pt[i].sectors_cnt, (pt[i].sectors_cnt * 512/1024)/1024);
		}
	}
}

void print_vbr_info(FILE* out, void* vbr, const uint8_t type)
{
	if (type == 0x01) {
		struct fat_vbr_sector* ptr = (struct fat_vbr_sector*) vbr;
		fprintf(out, "OEM: %s\n", ptr->bpb.oem_id);
		fprintf(out, "reserved sectors: %d\n", ptr->bpb.reserved_sectors);
		fprintf(out, "fat sectors size: %d\n", ptr->bpb.fat_cnt * ptr->bpb.sectors_per_fat);
		fprintf(out, "sector count: %d\n", ptr->bpb.sector_cnt);
		fprintf(out, "large sector count: %d\n", ptr->bpb.large_sector_cnt);
	}
}
