/*
 * Koosvc Utility
 *
 * Copyright (C) 2023 KOO Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <stdio.h>
#include <time.h>

void getfilename(char* dirname, char* filename)
{
	struct tm *ptm;

	time_t cur_time = time(NULL);
	ptm = localtime(&cur_time);

	sprintf(filename, "%s%04d-%02d%02d-%02d%02d-%02d.mp4", dirname, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

	printf("%s\n", filename);
}

