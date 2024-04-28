/*
 * OSG 
 *
 * Copyright (C) 2024 KOO Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <unistd.h>
#include <time.h>
#include <math.h>

#include <ft2build.h>
#include <freetype/freetype.h> 

extern void init_font(void);
extern void exit_font(void);

extern int set_enc_stamp_param(HD_PATH_ID stamp_path, UINT32 stamp_pa, UINT32 stamp_size);
extern int create_datetime_image(char *prefix);
extern int query_osg_buf_size(void);
extern int init_ftype(char *font_file, int font_size, double angle);
extern void *osg_thread(void *arg);
