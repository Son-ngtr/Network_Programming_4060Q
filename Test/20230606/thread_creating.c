#include <stdio.h>
#include <pthread.h>
void *thread_proc(void *);

int main() { 
    pthread_t thread_id;
    int param = 10;
    if (pthread_create(&thread_id, NULL, thread_proc, &param)) 
    {
        printf("Failed to create new thread\n");
        return 1;
    }   
    /* MAIN THREAD JOB */
    printf("Done !!!\n");
    return 0; 
}

// Hàm thực thi
void *thread_proc(void *arg) {
    /* CHILD THREAD JOB */
    printf(" %ld executing ...\n", pthread_self());
    return 0;
}
