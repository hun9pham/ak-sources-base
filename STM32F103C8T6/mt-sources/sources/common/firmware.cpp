#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "firmware.h"

int8_t firmwareBinGetInfo(firmwareHeader_t *firmwareHeader, const char *path) {
	int id;
	uint32_t Data;
	uint32_t Cs = 0;
	struct stat sfd;
	int fd = -1;

	fd = open(path, O_RDONLY);

	if (fd < 0) {
		return FILE_OPEN_FAILED;
	}

	fstat(fd, &sfd);

	for (id = 0; id < sfd.st_size; id += sizeof(uint32_t)) {
		Data = 0;
		int8_t status = pread(fd, &Data, sizeof(uint32_t), id);

		if(status == -1) {
			return FILE_READ_FAILED;
		}
		Cs += Data;
	}

	close(fd);

	firmwareHeader->binLen  = sfd.st_size;
	firmwareHeader->fCs = (uint16_t)(Cs & 0xFFFF);

    return FILE_NOERROR;
}

int8_t firmwareReadFile(uint8_t *buf, uint32_t cursor, uint32_t len, const char* path) {
	struct stat sfd;
	int fd = -1;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		return FILE_OPEN_FAILED;
	}

	fstat(fd, &sfd);

	if ((cursor + len) > (uint32_t)sfd.st_size) {
		return FILE_READ_OVERFLOW;
	}

	int8_t status = pread(fd, buf, len, cursor);
	if(status == -1) {
		return FILE_READ_FAILED;
	}
	close(fd);

	return FILE_NOERROR;
}
