#include <lib.h>	// provides _syscall and message
#include <unistd.h>	// this is where our function prototype is

int mdc(long a, long b) {
    message m;
    m.m1_i1 = a;
    m.m1_i2 = b;
    return _syscall(PM_PROC_NR, PM_MDC, &m);
}
