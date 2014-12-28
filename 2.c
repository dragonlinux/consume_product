#include <stdio.h>
#include <unistd.h>
#define mysleep(n) sleep(n)
void progressBar(int n)
{
    static int currentNumber = 0;
    if(n <= currentNumber)
        return;
    currentNumber = n;
    if (n == 1)
    {
        currentNumber = n;
        printf("=  %d",n);
        fflush(stdout);
        return;
    }
    if(n<11)
        printf("\b\b\b");
    else 
        printf("\b\b\b\b");
    printf("=  ");
    printf("%d", n);
    fflush(stdout);
}
int main()
{
    int i;
    for (i = 1; i <= 100; ++i)
    {
        progressBar(i);
        mysleep(1);
    }
    printf("/n");
    return 0;
}
