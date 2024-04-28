/*
 * KOO COMMON 
 *
 * Copyright (C) 2024 KOO Inc.
 *
 * Licensed under the GPL-2 or later.
 */
#include "hdal.h"
#include "hd_debug.h"

#define KOO_OSG

typedef struct _VIDEO_RECORD {

        // (1)
        HD_VIDEOCAP_SYSCAPS cap_syscaps;
        HD_PATH_ID cap_ctrl;
        HD_PATH_ID cap_path;

        HD_DIM  cap_dim;
        HD_DIM  proc_max_dim;

        // (2)
        HD_VIDEOPROC_SYSCAPS proc_syscaps;
        HD_PATH_ID proc_ctrl;
        HD_PATH_ID proc_path;

        HD_DIM  enc_max_dim;
        HD_DIM  enc_dim;

        // (3)
        HD_VIDEOENC_SYSCAPS enc_syscaps;
        HD_PATH_ID enc_path;

        // (4) user pull
        pthread_t  enc_thread_id;
#ifdef KOO_OSG
        pthread_t  osg_thread_id;
#endif

        UINT32     enc_exit;
        UINT32     flow_start;

#ifdef KOO_OSG
	HD_PATH_ID enc_stamp_path;

	UINT32 stamp_blk;
	UINT32 stamp_pa;
	UINT32 stamp_size;
#endif /* KOO_OSG */

} VIDEO_RECORD;

