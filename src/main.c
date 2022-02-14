#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "fatfs.h"

int main(int argc, char** argv)
{
	FILE* disk = NULL;
	struct mbr_sector* disk_mbr = NULL;
	struct fat_vbr_sector* part_vbr = NULL;
	int red, err;

	if (argc < 2) {
		printf("Syntax: %s <disk image>\n", argv[0]);
		return -1;
	}

	// opening disk
	disk = fopen(argv[1], "rb");
	assert(disk != NULL);
	
	// read the mbr sector
	disk_mbr = (struct mbr_sector*) malloc(sizeof(struct mbr_sector));
	red = read_mbr(disk_mbr, disk);
	assert(red > 0);

	// check if it's a valid boot record sector
	err = validate_mbr(disk_mbr);
	assert(!err);
	
	// print mbr info
	print_mbr_info(disk_mbr);

	// try to read the vbr sector
	part_vbr = (struct fat_vbr_sector*) malloc(sizeof(struct fat_vbr_sector)); 
	red = read_fat_vbr(part_vbr, disk_mbr->pt[0].start_lba_addr * 512, disk);
	assert(red > 0);

	// try to print some vbr info
	printf("OEM: %s\n", part_vbr->bpb.oem_id);
	printf("sector count: %d\n", part_vbr->bpb.sector_cnt);
	printf("large sector count: %d\n", part_vbr->bpb.large_sector_cnt);

	// free vbr sector
	free(part_vbr);
	// free mbr sector
	free(disk_mbr);
	// closing disk
	fclose(disk);

	return 0;
}
