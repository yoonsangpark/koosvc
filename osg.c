/*
 * OSG
 *
 * Copyright (C) 2024 KOO Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include "koocomm.h"
#include "osg.h"

#define WIDTH   1000
#define HEIGHT  100
unsigned short  dimage[HEIGHT * WIDTH];

#define posx	0
#define posy	1000

/* origin is the upper left corner */
FT_Library                      library = NULL;
FT_Face                         face = { 0 };
FT_GlyphSlot                    slot;
FT_Matrix                       matrix;                 /* transformation matrix */
FT_Vector                       pen;                    /* untransformed origin  */
FT_Error                        error;


///////////////////////////////////////////////////////////////////////////////

void init_font()
{
	char                *font_file;
	int                  font_size = 40;

	font_file = "/etc/fonts/DroidSansFallback.ttf";          
	if (init_ftype(font_file, font_size, 0)) {
		return;
	}
}

void exit_font()
{
	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

int init_ftype(char *font_file, int font_size, double angle)
{
	FT_Error error;

	if (!font_file) {
		printf("freetyped : invalid ftype parameter\n");
		return -1;
	}

	error = FT_Init_FreeType(&library);                /* initialize library */
	/* error handling omitted */

	error = FT_New_Face(library, font_file, 0, &face);   /* create face object */
	if (error) {
		printf("freetyped : FT_New_Face() fail with %d\r\n", error);
	}
	/* error handling omitted */

	/* use 50pt at 100dpi */
	error = FT_Set_Char_Size(face, font_size * 64, 0, 100, 0);                  /* set character size */
	/* error handling omitted */
	if (error) {
		printf("freetyped : FT_Set_Char_Size() fail with %d\r\n", error);
	}

	/* set up matrix */
	matrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
	matrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
	matrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
	matrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);

	return (int)error;
}

/* Replace this function with something useful. */
static void draw_bitmap(FT_Bitmap *bitmap, FT_Int x, FT_Int y, unsigned short *buf)
{
	FT_Int  i, j, p, q;
	FT_Int  x_max = x + bitmap->width;
	FT_Int  y_max = y + bitmap->rows;

	for (i = x, p = 0; i < x_max; i++, p++) {
		for (j = y, q = 0; j < y_max; j++, q++) {
			if (i < 0 || j < 0 || i >= WIDTH || j >= HEIGHT) {
				continue;
			}

			if (bitmap->buffer[q * bitmap->width + p])
				buf[j * WIDTH + i] = (((unsigned short)240 << 8) | (unsigned short)15);
			else
				buf[j * WIDTH + i] = 0x00;
		}
	}
}

int create_datetime_image(char *prefix)
{
	char                 new_date_time[50], osd_string[100];
	int                  n, num_dt, font_size = 20, width = 0, height = 0;
	time_t               tmp_time;
	struct tm            *timep;


	unsigned short *buf = dimage;

	memset(new_date_time, 0, sizeof(new_date_time));
	time(&tmp_time);
	timep = localtime(&tmp_time);
	if (timep) {
		sprintf(new_date_time, "%d/%.2d/%.2d - %.2d:%.2d:%.2d", 1900 + timep->tm_year, 1 + timep->tm_mon, timep->tm_mday, timep->tm_hour, timep->tm_min, timep->tm_sec);
	} else {
		printf("freetyped : localtime() fail\r\n");
		return -1;
	}

	if(prefix){
		if(strlen(prefix) < sizeof(osd_string))
			strcpy(osd_string, prefix);
		else{
			printf("size of prefix(%d) > size of osd string(%d)\r\n", strlen(prefix), sizeof(osd_string));
			return -1;
		}
	}

	if((strlen(prefix) + strlen(new_date_time)) < sizeof(osd_string))
		strcat(osd_string, new_date_time);
	else{
		printf("size of prefix(%d) + size of timestamp(%d) > size of osd string(%d)\r\n", strlen(prefix), strlen(new_date_time), sizeof(osd_string));
		return -1;
	}
	printf("new datetime is %s\n", osd_string);

	slot = face->glyph;

	memset(buf, 0, WIDTH * HEIGHT * 2);

	pen.x = 0;
	pen.y = 640;

	num_dt = strlen(osd_string);
	for (n = 0; n < num_dt; n++) {
		/* set transformation */
		FT_Set_Transform(face, &matrix, &pen);

		/* load glyph image into the slot (erase previous one) */
		error = FT_Load_Char(face, osd_string[n], FT_LOAD_RENDER);
		if (error) {
			printf("FT_Load_Char(%c) fail with %d\n", osd_string[n], error);
			return -1;
		}

		/* now, draw to our target surface (convert position) */
		draw_bitmap(&slot->bitmap, slot->bitmap_left, HEIGHT - slot->bitmap_top, buf);

		/* increment pen position */
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;

		height = slot->bitmap_top + (font_size - (slot->bitmap_top % font_size));
		width  = slot->bitmap_left + slot->bitmap.width;
		width  = width  + (font_size - (width % font_size));
		width += (4 - (width % 4));
	}

	if (width > WIDTH || height > HEIGHT) {
		printf("freetyped : image w(%d) h(%d) > max w(%d) h(%d)\r\n", width, height, WIDTH, HEIGHT);
		return -1;
	}

	return 0;
}

int query_osg_buf_size(void)
{
	HD_VIDEO_FRAME frame = {0};
	int stamp_size;

	frame.sign   = MAKEFOURCC('O','S','G','P');
			frame.dim.w  = WIDTH;
			frame.dim.h  = HEIGHT;
	frame.pxlfmt = HD_VIDEO_PXLFMT_ARGB4444;

	//get required buffer size for a single image
	stamp_size = hd_common_mem_calc_buf_size(&frame);
	if(!stamp_size){
		printf("fail to query buffer size\n");
		return -1;
	}

	//ping pong buffer needs double size
	stamp_size *= 2;

	return stamp_size;
}

int set_enc_stamp_param(HD_PATH_ID stamp_path, UINT32 stamp_pa, UINT32 stamp_size)
{
	HD_OSG_STAMP_BUF  buf;
	HD_OSG_STAMP_IMG  img;
	HD_OSG_STAMP_ATTR attr;

	if(!stamp_pa){
		printf("stamp buffer is not allocated\n");
		return -1;
	}

	memset(&buf, 0, sizeof(HD_OSG_STAMP_BUF));

	buf.type      = HD_OSG_BUF_TYPE_PING_PONG;
	buf.p_addr    = stamp_pa;
	buf.size      = stamp_size;

	if(hd_videoenc_set(stamp_path, HD_VIDEOENC_PARAM_IN_STAMP_BUF, &buf) != HD_OK){
		printf("fail to set stamp buffer\n");
		return -1;
	}

	memset(&img, 0, sizeof(HD_OSG_STAMP_IMG));

	img.fmt        = HD_VIDEO_PXLFMT_ARGB4444;
	img.dim.w      = WIDTH;
	img.dim.h      = HEIGHT;
	img.p_addr     = (UINT32)dimage;

	if(hd_videoenc_set(stamp_path, HD_VIDEOENC_PARAM_IN_STAMP_IMG, &img) != HD_OK){
		printf("fail to set stamp image\n");
		return -1;
	}

	memset(&attr, 0, sizeof(HD_OSG_STAMP_ATTR));

	attr.position.x = 0;
	attr.position.y = 240;
	attr.alpha      = 255;
	attr.layer      = 0;
	attr.region     = 0;

	return hd_videoenc_set(stamp_path, HD_VIDEOENC_PARAM_IN_STAMP_ATTR, &attr);
}

static int update_enc_stamp(HD_PATH_ID stamp_path, unsigned short *image)
{
	HD_OSG_STAMP_IMG  img;

	memset(&img, 0, sizeof(HD_OSG_STAMP_IMG));

	img.fmt        = HD_VIDEO_PXLFMT_ARGB4444;
	img.dim.w      = WIDTH;
	img.dim.h      = HEIGHT;
	img.p_addr     = (UINT32)image;

	return hd_videoenc_set(stamp_path, HD_VIDEOENC_PARAM_IN_STAMP_IMG, &img);
}

void *osg_thread(void *arg){

	VIDEO_RECORD* p_stream0 = (VIDEO_RECORD *)arg;

	while(p_stream0->enc_exit == 0) {
		if(create_datetime_image("Koo :")){
			printf("fail to create datetime image\n");
			continue;
		}
		if(update_enc_stamp(p_stream0->enc_stamp_path, dimage)){
			printf("fail to update datetime image\n");
			continue;
		}
		printf("datetime is updated\n");
		sleep(1);
	}

	return 0;
}
