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

#include "video_record.h"

#define KOO_IOCTL_MAGIC 'K'
#define KOOMON_START    _IOR(KOO_IOCTL_MAGIC, 0x1, int32_t*)
#define KOOMON_STOP     _IOR(KOO_IOCTL_MAGIC, 0x2, int32_t*)

#define BSIZE	100
#define MSIZE	3
char buf[BSIZE];

int main(int argc, char** argv)
{
	int fd;
	int ret = 0;

	fd = open("/dev/koomon", O_RDWR);
	if (fd < 0) {
		printf("File open error\n"); 
	}

	/* KOOMON_START */
	printf("koosvc : START \n");
	ret = ioctl(fd, KOOMON_START);
	printf("ioctl ret val (%d) errno (%d)\n", ret, errno);

	
	/* KOOMON Reading*/
	while ( 1 ) {

		printf("koosvc : Waiting ... \n");
		read(fd, buf, MSIZE );
		//printf(" >> %s\n", buf);
		
		
		/******************************/
		/* Recording Video */
		
		nvt_video_record();

		/******************************/
	}
	
	/* KOOMON_STOP */
	printf("koosvc : STOP \n");
	ret = ioctl(fd, KOOMON_STOP);
	printf("ioctl ret val (%d) errno (%d)\n", ret, errno);
	
	close(fd);
}
