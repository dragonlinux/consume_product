#include <stdio.h>

int main(void)
{
    int i=0;
    int j=0;
    for(;i<10;i++)
    {
        for(j=i;j<10;j++)
        {
            printf("1");
        }
       // printf("\n");
        fflush(stdout);
        sleep(1);
    }
    return 0;
}
