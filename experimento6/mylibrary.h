//
//  Created on May 23rd 2017
//
//  ALEX VENTURINI  15294739
//  BRUNO PEDROSO   12662136
//  LUAN BONOMI     15108780
//  PEDRO CATALINI  15248354
//  ROGER OBA       12048534
//

#include <lib.h>

int whosthere() {
    message m;
    return _syscall(PM_PROC_NR, WHOSTHERE, &m);
}

int mdc(long a, long b) {
    message m;
    m.m1_i1 = a;
    m.m1_i2 = b;
    return _syscall(PM_PROC_NR, PM_MDC, &m);
}