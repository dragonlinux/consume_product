#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include<semaphore.h>
#include <stdlib.h>
#include "ltpro.h"


#define TOTAL 5
#define INIT 2

sem_t *full,*empty,*muxtex,*number;

int num = 0;
int filedes[2];


void process_bar(char * str)
{
    progress_t bar;
    progress_init(&bar, str, 50, PROGRESS_BGC_STYLE);
    int i;
    for ( i = 0; i <= 50; i++ ) {
	progress_show(&bar, i/50.0f);
	usleep(60000);
    }
    printf("\n");
    progress_destroy(&bar);

}

void product()
{
    printf("Product ......\n"); 
    process_bar("product");
}
void consume()
{
    printf("Consume .......\n"); 
    process_bar("consume");
}

int SubmitPruductBuffer()
{
    sem_wait(number);
    read(filedes[0],&num,sizeof(num));
    num++;
    write(filedes[1],&num,sizeof(num));
    sem_post(number);
    printf("Submit ........\n");
    process_bar("submit");
    printf("Now the buffer has %d products\n",num);
    return num;
}
int GetProductBuffer()
{
    sem_wait(number);
    read(filedes[0],&num,sizeof(num));
    num--;
    write(filedes[1],&num,sizeof(num));
    sem_post(number);
    printf("Get .......\n");
    process_bar("get");
    printf("Now the buffer has %d products\n",num);
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

    int p1;
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

    while((p1=fork())==-1);
    if(p1!=0)
    {
        write(filedes[1],&num,sizeof(num));
        producer();
    }
    else
    {
        consumer();
    }

    return 0;
}
