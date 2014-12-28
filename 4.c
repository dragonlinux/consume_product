#include <stdio.h>
#include <unistd.h>

void print(i)
{
    int j=0;
    for(;j<i;j++)
    {
        printf("1");
    }
    fflush(stdout);
}

int main(void)
{
    
    int i=1;
    for(;i<10;i++)
    {
        print(i);
        sleep(1);
    }
    return 0;

}
