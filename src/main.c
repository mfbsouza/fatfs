#include <stdio.h>

#include "fatfs.h"

int main(int argc, char** argv)
{
	// check arguments given to the code
	if (argc < 2) {
		printf("Syntax: %s <disk image>\n", argv[0]);
		return -1;
	}

	struct fatfs_t fat;

	fatfs_init(&fat, argv[1]);
	fatfs_print_info(&fat, stdout);
	fatfs_exit(&fat);

	return 0;
}
