#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    if(fork() == 0){
        printf("CP %d started n\n", getpid());
        sleep(10);
        printf("CP %d dont \n", getpid());
        exit(0);
    }
    printf("MP %d done \n");
    return 0;
}
