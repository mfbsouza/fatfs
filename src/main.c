#include <stdio.h>

#include "fatfs.h"

int main(int argc, char** argv)
{
	if (argc < 2) {
		printf("Syntax: %s <disk image>\n", argv[0]);
		return -1;
	}

	FILE* disk = fopen(argv[1], "rb");
	if (!disk) {
		fprintf(stderr, "Cannot open disk image %s!\n", argv[1]);
		return -1;
	}

	struct mbr_sector mbr;
	if (!read_boot_sector(&mbr, disk)) {
		fprintf(stderr, "Cannor read boot sector!\n");
		return -1;
	}

	printf("parition 1 type: %x\n", mbr.pt_entry[0].type);

	fclose(disk);

	return 0;
}
