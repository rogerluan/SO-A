#include <stdio.h>

int main(void) {
    int n;        /* O numero de numeros da seria de fibonacci que serão exibidos */
    int i;        /* O indice do proximo numero da serie de fibonacci a ser exibido */
    int current;  /* O valor do (i)esimo numero de fibonacci */
    int next;     /* O valor do (i+1)esimo numero de fibonacci */
    int twoaway;  /* O valor do (i+2)esimo numero de fibonacci */
    printf("Quantos numeros da serie de Fibonacci sao desejados?");
    scanf("%d", &n);
    printf("\n\n");
    if (n <= 0)
        printf("O numero deve ser positivo.n");
    else {
        next = current = 1;
        for (i=1; i<=n; i++) {
            printf("%d ",current);
            twoaway = current+next;
            current = next;
            next = twoaway;
        }
    }
    printf("\n\n");
}
