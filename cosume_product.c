#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include<semaphore.h>
#include <stdlib.h>
#include "ltpro.h"


#define TOTAL 5
#define INIT 2
#define SLEEP 5000

sem_t *full,*empty,*muxtex,*number;

int num = 0;
int filedes[2];

float bar_pro,bar_cos,bar_sub,bar_get;
int fbar_pro[2],fbar_cos[2],fbar_sub[2],fbar_get[2];


void process_bar()
{
    char * title[4] = {"product ","consume ","submit  ","get     "};
    float data[4];
    printf("\n\n");
    progress_t bar;

    progress_init(&bar,4,title, 50, PROGRESS_BGC_STYLE);
    while(1)
    {
        read(fbar_pro[0],&bar_pro,sizeof(bar_pro));
        read(fbar_cos[0],&bar_cos,sizeof(bar_cos));
        read(fbar_sub[0],&bar_sub,sizeof(bar_sub));
        read(fbar_get[0],&bar_get,sizeof(bar_get));
        read(filedes[0],&num,sizeof(num));
        data[0] = bar_pro;
        data[1] = bar_cos;
        data[2] = bar_sub;
        data[3] = bar_get;
        progress_show(&bar, data,num);
        write(filedes[1],&num,sizeof(num));
        write(fbar_pro[1],&bar_pro,sizeof(bar_pro));
        write(fbar_cos[1],&bar_cos,sizeof(bar_cos));
        write(fbar_sub[1],&bar_sub,sizeof(bar_sub));
        write(fbar_get[1],&bar_get,sizeof(bar_get));
 
        usleep(SLEEP);
    }
    progress_destroy(&bar);

}

void product()
{
    int i=0;
    for(;i<50;i++)
    {
        read(fbar_pro[0],&bar_pro,sizeof(bar_pro));
        bar_pro = i/50.0f;
        write(fbar_pro[1],&bar_pro,sizeof(bar_pro));
        usleep(SLEEP*4);
    }
    read(fbar_pro[0],&bar_pro,sizeof(bar_pro));
    bar_pro = 0;
    write(fbar_pro[1],&bar_pro,sizeof(bar_pro));

}
void consume()
{
    int i=0;
    for(;i<50;i++)
    {
        read(fbar_cos[0],&bar_pro,sizeof(bar_pro));
        bar_pro = i/50.0f;
        write(fbar_cos[1],&bar_pro,sizeof(bar_pro));
        usleep(SLEEP*8);
    }
        read(fbar_cos[0],&bar_pro,sizeof(bar_pro));
        bar_pro = 0;
        write(fbar_cos[1],&bar_pro,sizeof(bar_pro));
}

int SubmitPruductBuffer()
{
    int i=0;
    
    for(;i<50;i++)
    {
        read(fbar_sub[0],&bar_pro,sizeof(bar_pro));
        bar_pro = i/50.0f;
        write(fbar_sub[1],&bar_pro,sizeof(bar_pro));
        usleep(SLEEP*2);
    }
        read(fbar_sub[0],&bar_pro,sizeof(bar_pro));
        bar_pro = 0;
        write(fbar_sub[1],&bar_pro,sizeof(bar_pro));
        sem_wait(number);
    read(filedes[0],&num,sizeof(num));
    num++;
    write(filedes[1],&num,sizeof(num));
    sem_post(number);
    return num;
}
int GetProductBuffer()
{
    int i=0;
    sem_wait(number);
    read(filedes[0],&num,sizeof(num));
    num--;
    write(filedes[1],&num,sizeof(num));
    sem_post(number);
    for(;i<50;i++)
    {
        read(fbar_get[0],&bar_pro,sizeof(bar_pro));
        bar_pro = i/50.0f;
        write(fbar_get[1],&bar_pro,sizeof(bar_pro));
        usleep(SLEEP*2);
    }
        read(fbar_get[0],&bar_pro,sizeof(bar_pro));
        bar_pro = 0;
        write(fbar_get[1],&bar_pro,sizeof(bar_pro));
    return num;
}

void producer()
{
    while(1)
    {
        product();
        sem_wait(empty);
        sem_wait(muxtex);
        SubmitPruductBuffer();
        sem_post(muxtex);
        sem_post(full);
    }

}


void consumer()
{
    while(1)
    {
        sem_wait(full);
        sem_wait(muxtex);
        GetProductBuffer();
        sem_post(muxtex);
        sem_post(empty);
        consume();
    }
}

void killfun()
{
    sem_unlink("full");
    sem_unlink("empty");
    sem_unlink("muxtex");
    sem_unlink("number");
    exit(0);
}



int main(void)
{

    int p1,p2;
    signal(SIGINT,killfun);
    full = sem_open("full",O_CREAT,S_IRWXU,0);
    empty = sem_open("empty",O_CREAT,S_IRWXU,5);
    muxtex = sem_open("muxtex",O_CREAT,S_IRWXU,1);
   // pthread_create(&ptd1,NULL,consumer,NULL);
   // pthread_create(&ptd2,NULL,producer,NULL);
   // pthread_join(ptd1,NULL);
   // pthread_join(ptd2,NULL);
    number = sem_open("number",O_CREAT,S_IRWXU,1);
    pipe(filedes);
    pipe(fbar_pro);
    pipe(fbar_cos);
    pipe(fbar_sub);
    pipe(fbar_get);
    bar_pro = 0;
    bar_cos = 0;
    bar_sub = 0;
    bar_get = 0;
    while((p1=fork())==-1);
    if(p1!=0)
    {
        while((p2=fork())==-1);
        if(p2!=0)
        {
            write(fbar_pro[1],&bar_pro,sizeof(bar_pro));
            write(fbar_cos[1],&bar_cos,sizeof(bar_cos));
            write(fbar_sub[1],&bar_sub,sizeof(bar_sub));
            write(fbar_get[1],&bar_get,sizeof(bar_get));
            write(filedes[1],&num,sizeof(num));
            
            producer();
        }
        else
        {
            process_bar();
        }
    }
    else
    {
        consumer();
    }
    return 0;
}
