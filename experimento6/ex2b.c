#include <stdio.h> // printf()
#include <stdlib.h> // atoi()
#include "customlib.h" //mdc()

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("This method expects 2 integers as parameters.\nExiting... \n");
        exit(1); // it should expect 2 params
    }
    long a = atoi(argv[1]);
    long b = atoi(argv[2]);
    return mdc(a, b);
}
