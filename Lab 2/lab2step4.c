//Name: Vincent Yu
//Date: 04/07/2020
//Title: Lab 2- Part 2 Step 4
//Description: This program computes the answers for questions in Part 2 Step 4 of Lab 2.

#include <stdio.h>      
#include <unistd.h>   
#include <stdlib.h>     
#include <math.h>

int main(int argc, char *argv[]) {
    int n = atoi(argv[1]);
    int objects = 6;

    int RTT0 = 3;
    int RTT1 = 20;
    int RTT2 = 26;
    int RTTHTTP = 47;

    int a = RTT0 + RTT1 + RTT2 + (2*RTTHTTP);
    printf("4a: %dms \n", a);

    int b = RTT0 + RTT1 + RTT2 + (2*RTTHTTP) + (2*(objects*RTTHTTP));
    printf("4b: %dms \n", b);

    int c_p = RTT0 + RTT1 + RTT2 + (2*RTTHTTP) + (2*((ceil(objects/n)+1)*RTTHTTP));
    printf("4c persistent: %dms \n", c_p);

    int c_np = RTT0 + RTT1 + RTT2 + (2*RTTHTTP) + RTTHTTP;
    printf("4c non-persistent: %dms \n", c_np);    

    return 0;
}