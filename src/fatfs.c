#include "fatfs.h"

size_t read_mbr(struct mbr_sector* buf, FILE* disk)
{
	return fread(buf, sizeof(struct mbr_sector), 1, disk);
}

int validate_mbr(struct mbr_sector* mbr)
{
	if (mbr->signature[0] == 0x55 && mbr->signature[1] == 0xaa) {
		return 0;
	} else {
		fprintf(stderr, "invalid mbr sector\n");
		return -1;
	}
}

int count_partitions(struct mbr_sector* mbr)
{
	int i, cnt = 0;
	for (i = 0; i < MAX_MBR_PARTITIONS; i++) {
		if (mbr->pt_entry[i].type != 0) {
			cnt++;
		}
	}
	return cnt;
}

void print_mbr_info(struct mbr_sector* mbr)
{
	int i, cnt = 0;
	cnt = count_partitions(mbr);
	fprintf(stdout, "partitions on the disk: %d\n", cnt);
	for (i = 0; i < cnt; i++) {
		fprintf(stdout, "parition %d type: %2.x\n", i, mbr->pt_entry[i].type);
	}
}
