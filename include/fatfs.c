#include "fatfs.h"

size_t read_boot_sector(struct mbr_sector* buf, FILE* disk)
{
	size_t ret = fread(buf, sizeof(struct mbr_sector), 1, disk);
	return ret;
}
