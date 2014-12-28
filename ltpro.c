/**
 * linux terminal progress bar (no thread safe).
 * 	@package progress.c
 *
 * @author chenxin <chenxin619315@gmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ltpro.h"

/**
 * initialize the progress bar.
 * @max = 0
 * @val = 0
 *
 * @param	style
 * @param	tip words.
 */
extern void progress_init(
	progress_t *bar, char *title1,char * title2,char * title3,char * title4, int max, int style)
{
    bar->chr = '#';
    bar->title1 = title1;
    bar->title2 = title2;
    bar->title3 = title3;
    bar->title4 = title4;
    bar->style = style;
    bar->max = max;
    bar->offset = 100 / (float)max;
    bar->pro1 = (char *) malloc(max+1);
    bar->pro2 = (char *) malloc(max+1);
    bar->pro3 = (char *) malloc(max+1);
    bar->pro4 = (char *) malloc(max+1);
    if ( style == PROGRESS_BGC_STYLE )
	memset(bar->pro1, 0x00, max+1);
    else {
	memset(bar->pro1, 32, max);
	memset(bar->pro1+max, 0x00, 1);
	memset(bar->pro2, 32, max);
	memset(bar->pro2+max, 0x00, 1);
	memset(bar->pro3, 32, max);
	memset(bar->pro3+max, 0x00, 1);
	memset(bar->pro4, 32, max);
	memset(bar->pro4+max, 0x00, 1);
    }
}

extern void progress_show( progress_t *bar, float bit1, float bit2,float bit3,float bit4)
{
    int val1 = (int)(bit1 * bar->max);
    int val2 = (int)(bit2 * bar->max);
    int val3 = (int)(bit3 * bar->max);
    int val4 = (int)(bit4 * bar->max);
    switch ( bar->style ) 
    {
    case PROGRESS_NUM_STYLE:
	printf("\033[?25l\033[31m\033[1m%s%d%%\033[?25h\033[0m\r",
		bar->title1, (int)(bar->offset * val1));
	fflush(stdout);
	break;
    case PROGRESS_CHR_STYLE:
	memset(bar->pro1, '#', val1);
	printf("\033[?25l\033[31m\033[1m%s[%-s] %d%%\033[?25h\033[0m\r", 
		bar->title1, bar->pro1, (int)(bar->offset * val1));
	fflush(stdout);
	break;
    case PROGRESS_BGC_STYLE:
    memset(bar->pro1, 0x00,bar->max);
    memset(bar->pro2, 0x00,bar->max);
    memset(bar->pro3, 0x00,bar->max);
    memset(bar->pro4, 0x00,bar->max);
	memset(bar->pro1, 32, val1);
	memset(bar->pro2, 32, val2);
	memset(bar->pro3, 32, val3);
	memset(bar->pro4, 32, val4);
	printf(/*\033[K\033[1A*/"\033[K\033[1A\033[K\033[1A\033[K\033[1A\033[K\033[?25l\033[32m\033[1m%s\033[44m %d%% %s\033[?25h\033[0m\n\
\033[?25l\033[32m\033[1m%s\033[43m %d%% %s\033[?25h\033[0m\n\
\033[?25l\033[32m\033[1m%s\033[42m %d%% %s\033[?25h\033[0m\n\
\033[?25l\033[32m\033[1m%s\033[41m %d%% %s\033[?25h\033[0m\r",
//product number = %d\r", 
    bar->title1, (int)(bar->offset * val1), bar->pro1,bar->title2, (int)(bar->offset * val2), bar->pro2,
    bar->title3, (int)(bar->offset * val3), bar->pro3,
    bar->title4, (int)(bar->offset * val4), bar->pro4);
    fflush(stdout);
	break;
    }
}

//destroy the the progress bar.
extern void progress_destroy(progress_t *bar)
{
    free(bar->pro1);
    free(bar->pro2);
    free(bar->pro3);
    free(bar->pro4);
}
