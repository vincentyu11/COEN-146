//Name: Vincent Yu
//Date: March 31 2:15-5:00pm
//Title: Lab 1 - Step 5
//Description: This program uses threads to mirror what the sample program does.


/*COEN 146L : Lab1 */
/*Sample C program for Lab assignment 1*/
#include <stdio.h>      /* printf, stderr */
#include <sys/types.h>  /* pid_t */
#include <unistd.h>     /* fork */
#include <stdlib.h>     /* atoi */
#include <errno.h>      /* errno */ 
#include <pthread.h>
/* main function with command-line arguments to pass */


void *threadPrint(void *threadid) {
    int i;
    for (i=0; i<100; i++) {
        printf("Thread %d \n", i);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    
    pthread_t thread1;
    pthread_t thread2;
    int thread;
    
    pthread_create(&thread1, NULL, threadPrint, NULL);
    pthread_create(&thread2, NULL, threadPrint, NULL);

    
    //pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    pthread_exit(NULL);

    return 0;
}