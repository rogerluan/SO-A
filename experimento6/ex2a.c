//
//  Created on May 23rd 2017
//
//  ALEX VENTURINI  15294739
//  BRUNO PEDROSO   12662136
//  LUAN BONOMI     15108780
//  PEDRO CATALINI  15248354
//  ROGER OBA       12048534
//

#include <stdio.h> // printf()
#include <stdlib.h> // atoi()
#include <lib.h> // _syscall and message

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("This method expects 2 integers as parameters.\nExiting... \n");
        exit(1); // it should expect 2 params
    }
    message m;
    m.m1_i1 = atoi(argv[1]);
    m.m1_i2 = atoi(argv[2]);
    return _syscall(PM_PROC_NR, PM_MDC, &m);
}
