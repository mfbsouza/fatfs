#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "fatfs.h"

int main(int argc, char** argv)
{
	FILE* disk = NULL;
	struct partition_table* pt = NULL;
	struct fat_vbr_sector* vbr = NULL;
	int err;

	// check arguments given to the code
	if (argc < 2) {
		printf("Syntax: %s <disk image>\n", argv[0]);
		return -1;
	}

	// opening disk
	disk = fopen(argv[1], "rb");
	assert(disk != NULL);

	// check if disk has a valid MBR sector
	err = validate_mbr(disk);
	assert(!err);

	// load the partition tables to memory
	pt = load_part_table(disk);

	// print partition table info
	print_pt_info(stdout, pt);

	// load VBR sector
	vbr = (struct fat_vbr_sector*) load_sector(pt[0].start_lba_addr * SECTOR_SIZE, 1, disk);

	// print VBR info
	print_vbr_info(stdout, vbr, pt[0].type);

	// free vbr sector
	free(vbr);

	// free the partition tables
	free(pt);

	// close the disk
	fclose(disk);

	return 0;
}
