#include <stdio.h>
#include "pm.h"
 
int calculate_gcd_r(long a, long b);
int calculate_gcd(long a, long b);

int do_mdc() {
    /** m_in is a global variable set to PM's incoming message, 
     *  So m_in.m1_i1 is the integer parameter set in the gcd program.
     */
    int a = m_in.m1_i1;
    int b = m_in.m1_i2;
    int result = 0;
    if (a == 0 || b == 0) {
	printf("One or both of the given numbers are equal to zero. Quitting . . ."); 
        return 0;
    }
    result = calculate_gcd(a, b);
    printf("Greatest Common Divisor is: %ld\n", (long)result);
    return 1; 
}

int calculate_gcd(long a, long b) {
    int temp;
    while (b != 0) {
        temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

int calculate_gcd_r(long a, long b) {
    return b == 0 ? a : calculate_gcd_r(b, a % b);
}
