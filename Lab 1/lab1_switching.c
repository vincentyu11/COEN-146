//Name: Vincent Yu
//Date: March 31 2:15-5:00pm
//Title: Lab 1 - Step 6-7
//Description: This program demonstrates circuit switching and packet switching.

//gcc lab1_switching.c -o lab1_switching -lm

#include <stdio.h>      
#include <unistd.h>   
#include <stdlib.h>     
#include <math.h>

double factorial (double number) {
    if (number == 0) {
        return 1;
    }
    return number * factorial(number-1);
}

int main(int argc, char *argv[]) {
    
    int linkBandwidth = 200;

    int userBandwidth = 20;

    //6a
    int nCSusers = linkBandwidth/userBandwidth;
    printf("6a: %d\n", nCSusers);

    int nPSusers = 19;

    double tPSuser = 0.10;

    //7a
    double pPSusers = tPSuser;
    printf("7a: %.13f\n", pPSusers);

    double pPSusersBusy;

    //7b
    double pPSusersNotBusy = 1 - pPSusers;
    printf("7b: %.13f\n", pPSusersNotBusy);

    //7c
    double c = pow((1-pPSusers),(nPSusers-1));
    printf("7c: %.13f\n", c);

    //7d
    double d = pPSusers * pow(pPSusersNotBusy,nPSusers-1);
    printf("7d: %.13f\n", d);

    //7e
    double e = nPSusers * (pPSusers * pow(pPSusersNotBusy,nPSusers-1));
    printf("7e: %.13f\n", e);

    //7f
    double f = pow(pPSusers,10) * pow(pPSusersNotBusy,nPSusers-10);
    printf("7f: %.13f\n", f);

    //7g
    double g;
    double gnum = factorial(nPSusers);
    double gden = factorial(10)*factorial(nPSusers-10);
    g = gnum/gden;
    g = g*f;
    printf("7g: %.13f\n", g);

    //7h
    double h = 0;
    int i;
    for (i=11; i<=nPSusers; i++) {
        double tempnum = factorial(nPSusers);
        double tempden = factorial(i)*factorial(nPSusers-i);
        double temp = tempnum/tempden;
        temp = temp * pow(pPSusers,i) * pow(pPSusersNotBusy,nPSusers-i);
        h = h + temp;
    }
    printf("7h: %.13f\n", h);


    return 0;
}