#include <stdio.h>
#include <unistd.h>
int main() { 
    int i = 5;
    printf("Before FORK %d\n", i);
    int cid = fork();
    int cid2 = fork();
    i++;
    printf("After FORK, PID = %d %d %d\n", cid, cid2, i);
    
    return 0; 
}