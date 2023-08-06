/*
 * Driver for the Koomon
 *
 * Copyright (C) 2023 KOO Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>


#define KOO_IOCTL_MAGIC 'K'
#define KOOMON_START    _IOR(KOO_IOCTL_MAGIC, 0x1, int32_t*)
#define KOOMON_STOP     _IOR(KOO_IOCTL_MAGIC, 0x2, int32_t*)


int main(int argc, char** argv)
{
	int fd;
	int ret = 0;

	fd = open("/dev/koomon", O_RDWR);
	if (fd < 0) {
		printf("File open error\n"); 
	}

	/* KOOMON_START */
	printf("Sending KOOMON_START\n");
	ret = ioctl(fd, KOOMON_START);
	printf("ioctl ret val (%d) errno (%d)\n", ret, errno);
	
	/* KOOMON_STOP */
	printf("Sending KOOMON_STOP\n");
	ret = ioctl(fd, KOOMON_STOP);
	printf("ioctl ret val (%d) errno (%d)\n", ret, errno);

	close(fd);
}