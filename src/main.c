#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "fatfs.h"

int main(int argc, char** argv)
{
	FILE* disk = NULL;
	struct mbr_sector* disk_mbr = NULL;
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

	// free mbr sector
	free(disk_mbr);

	// closing disk
	fclose(disk);

	return 0;
}
