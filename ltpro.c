#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ltpro.h"

/**
 * initialize the progress bar.
 */
extern void progress_init(
	progress_t *bar,int title_num, char *titles[], int max, int style)
{
    int i;
    bar->chr = '#';
    bar->number = title_num;
    bar->title = (char **)malloc(sizeof(char *)*title_num);
    bar->pro = (char **)malloc(sizeof(char *)*title_num);
    bar->style = style;
    bar->max = max;
    for(i =0;i < title_num;i++)
    {
        bar->pro[i] = (char *) malloc(max+1);
        bar->title[i] = titles[i];
        memset(bar->pro[i], 0x00, max+1);
    }
    bar->offset = 100 / (float)max;
}

extern void progress_show( progress_t *bar,float *bit, int product)
{
    int number = bar->number;
    int *val = (int *)malloc(sizeof(int)*number);
    int i;
    for(i = 0;i < number;i++)
    {
        val[i] = (int)(bit[i] * bar->max);
        memset(bar->pro[i],0x00,bar->max);
        memset(bar->pro[i], 32, val[i]);
    }

    printf("\033[K");
    for(i=0;i<number;i++)
    {
        printf("\033[1A\033[K");
    }
    for(i=0;i<number;i++)
    {
        printf("\033[?25l\033[32m\033[1m%s\033[43m %d%% %s\033[?25h\033[0m\n",
    bar->title[i], (int)(bar->offset * val[i]), bar->pro[i]);
    }
    printf("product number = %d\r",product);
    fflush(stdout);

    free(val);
}

//destroy the the progress bar.
extern void progress_destroy(progress_t *bar)
{

    int i;
    int number = bar->number;
    for(i = 0;i < number ; i++)
    {
        free(bar->pro[i]);
    }
    free(bar->pro);
    free(bar->title);
}
