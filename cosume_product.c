#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include<semaphore.h>
#include <stdlib.h>
#include "ltpro.h"


#define TOTAL 5
#define INIT 2
#define MAXPRO 10
#define SLEEP 30000
#define CONSUME 2
#define PRODUCT 5

sem_t *full,*empty,*muxtex,*number;

int num = 0;
int filedes[2];
progress_t bar;

float bar_pro[MAXPRO],bar_cos[MAXPRO],bar_sub[MAXPRO],bar_get[MAXPRO];
int fbar_pro[MAXPRO][2],fbar_cos[MAXPRO][2],fbar_sub[MAXPRO][2],fbar_get[MAXPRO][2];


void killfun()
{
    sem_unlink("full");
    sem_unlink("empty");
    sem_unlink("muxtex");
    sem_unlink("number");
    progress_destroy(&bar);
    exit(0);
}

void process_bar(int number_pro,int number_con)
{
    int number = (number_pro + number_con)*2;
    char ** titles = (char**)malloc(sizeof(char *)*number);
    float *data = (float*)malloc(sizeof(float)*number);
    int i = 0;
    for(i = 0;i<number_pro;i++)
    {
        titles[i*2] = "product ";
        titles[i*2+1] = "submit  ";
    }
    for(i = 0; i<number_con;i++)
    {
        titles[(i+number_pro)*2] = "consume ";
        titles[(i+number_pro)*2+1] = "get     ";
    }
    
    printf("\n\n");

    progress_init(&bar,number,titles, 50, PROGRESS_BGC_STYLE);
    while(1)
    {
        for(i=0;i<number_pro;i++)
        {
            read(fbar_pro[i][0],&bar_pro[i],sizeof(bar_pro[i]));
            data[i*2] = bar_pro[i];
            write(fbar_pro[i][1],&bar_pro[i],sizeof(bar_pro[i]));
            read(fbar_sub[i][0],&bar_sub[i],sizeof(bar_sub[i]));
            data[i*2+1] = bar_sub[i];
            write(fbar_sub[i][1],&bar_sub[i],sizeof(bar_sub[i]));

        }
        for(i=0;i<number_con;i++)
        {
            read(fbar_cos[i][0],&bar_cos[i],sizeof(bar_cos[i]));
            data[(i+number_pro)*2] = bar_cos[i];
            write(fbar_cos[i][1],&bar_cos[i],sizeof(bar_cos[i]));
            read(fbar_get[i][0],&bar_get[i],sizeof(bar_get[i]));
            data[(i+number_pro)*2+1] = bar_get[i];
            write(fbar_get[i][1],&bar_get[i],sizeof(bar_get[i]));

        }
        read(filedes[0],&num,sizeof(num));
        progress_show(&bar, data,num);
        write(filedes[1],&num,sizeof(num));
        usleep(SLEEP);
    }

}

void product(int pos)
{
    int i=0;
    for(;i<50;i++)
    {
        read(fbar_pro[pos][0],&bar_pro[pos],sizeof(bar_pro[pos]));
        bar_pro[pos] = i/50.0f;
        write(fbar_pro[pos][1],&bar_pro[pos],sizeof(bar_pro[pos]));
        usleep(SLEEP*4);
    }
    read(fbar_pro[pos][0],&bar_pro[pos],sizeof(bar_pro[pos]));
    bar_pro[pos] = 0;
    write(fbar_pro[pos][1],&bar_pro[pos],sizeof(bar_pro[pos]));

}
void consume(int pos)
{
    int i=0;
    for(;i<50;i++)
    {
        read(fbar_cos[pos][0],&bar_cos[pos],sizeof(bar_cos[pos]));
        bar_cos[pos] = i/50.0f;
        write(fbar_cos[pos][1],&bar_cos[pos],sizeof(bar_cos[pos]));
        usleep(SLEEP*8);
    }
        read(fbar_cos[pos][0],&bar_cos[pos],sizeof(bar_cos[pos]));
        bar_cos[pos] = 0;
        write(fbar_cos[pos][1],&bar_cos[pos],sizeof(bar_cos[pos]));
}

int SubmitPruductBuffer(int pos)
{
    int i=0;
    
    for(;i<50;i++)
    {
        read(fbar_sub[pos][0],&bar_sub[pos],sizeof(bar_sub[pos]));
        bar_sub[pos] = i/50.0f;
        write(fbar_sub[pos][1],&bar_sub[pos],sizeof(bar_sub[pos]));
        usleep(SLEEP*2);
    }
    read(fbar_sub[pos][0],&bar_sub[pos],sizeof(bar_sub[pos]));
    bar_sub[pos] = 0;
    write(fbar_sub[pos][1],&bar_sub[pos],sizeof(bar_sub[pos]));
    sem_wait(number);
    read(filedes[0],&num,sizeof(num));
    num++;
    write(filedes[1],&num,sizeof(num));
    sem_post(number);
    return num;
}
int GetProductBuffer(int pos)
{
    int i=0;
    sem_wait(number);
    read(filedes[0],&num,sizeof(num));
    num--;
    write(filedes[1],&num,sizeof(num));
    sem_post(number);
    for(;i<50;i++)
    {
        read(fbar_get[pos][0],&bar_get[pos],sizeof(bar_get[pos]));
        bar_get[pos] = i/50.0f;
        write(fbar_get[pos][1],&bar_get[pos],sizeof(bar_get[pos]));
        usleep(SLEEP*2);
    }
    read(fbar_get[pos][0],&bar_get[pos],sizeof(bar_get[pos]));
    bar_get[pos] = 0;
    write(fbar_get[pos][1],&bar_get[pos],sizeof(bar_get[pos]));
    return num;
}

void producer(int pos)
{
    while(1)
    {
        product(pos);
        sem_wait(empty);
        sem_wait(muxtex);
        SubmitPruductBuffer(pos);
        sem_post(muxtex);
        sem_post(full);
    }

}


void consumer(int pos)
{
    while(1)
    {
        sem_wait(full);
        sem_wait(muxtex);
        GetProductBuffer(pos);
        sem_post(muxtex);
        sem_post(empty);
        consume(pos);
    }
}



int main(void)
{

    int p[10];
    signal(SIGINT,killfun);
    int i=0;
    full = sem_open("full",O_CREAT,S_IRWXU,0);
    empty = sem_open("empty",O_CREAT,S_IRWXU,5);
    muxtex = sem_open("muxtex",O_CREAT,S_IRWXU,1);
   // pthread_create(&ptd1,NULL,consumer,NULL);
   // pthread_create(&ptd2,NULL,producer,NULL);
   // pthread_join(ptd1,NULL);
   // pthread_join(ptd2,NULL);
    number = sem_open("number",O_CREAT,S_IRWXU,1);
    pipe(filedes);
    for(i=0;i<PRODUCT;i++)
    {
        pipe(fbar_sub[i]);
        pipe(fbar_pro[i]);
        bar_pro[i] = 0;
        bar_sub[i] = 0;
        write(fbar_pro[i][1],&bar_pro[i],sizeof(bar_pro[i]));
        write(fbar_sub[i][1],&bar_sub[i],sizeof(bar_sub[i]));
    }
    for(i=0;i<CONSUME;i++)
    {
        pipe(fbar_cos[i]);
        pipe(fbar_get[i]);
        bar_cos[i] = 0;
        bar_get[i] = 0;
        write(fbar_cos[i][1],&bar_cos[i],sizeof(bar_cos[i]));
        write(fbar_get[i][1],&bar_get[i],sizeof(bar_get[i]));
    }
    write(filedes[1],&num,sizeof(num));
    for(i=0;i<CONSUME+PRODUCT;i++)
    {
        while((p[i]=fork())==-1);
        if(p[i]==0)
            break;
        if(i==CONSUME+PRODUCT-1)
        {
            printf("%d\n",getpid());
            consumer(CONSUME-1);
        }
    }
    
    if(i<PRODUCT)
    {
        printf("%d\n",getpid());
        producer(i);
    }
    else if(i<PRODUCT+CONSUME-1)
    {
        printf("%d\n",getpid());
        consumer(i-PRODUCT);
    }
    else
        process_bar(PRODUCT,CONSUME);
        
    return 0;
}
