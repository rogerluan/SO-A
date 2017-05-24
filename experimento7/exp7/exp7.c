//
//  exp7.c
//
//  Created on 22/05/17
//
//  ALEX VENTURINI  15294739
//  BRUNO PEDROSO   12662136
//  LUAN BONOMI     15108780
//  PEDRO CATALINI  15248354
//  ROGER OBA       12048534
//

// Open
// -function 1 -input "path"

// Write
// -function 2 -input "lorem ipsum" -output "path" -dataSize 1000

// Read
// -function 3 -input "path" -output "path" -dataSize 1000

// Seek
// -function 4 -input "path" -offset 123

// Close
// -function 5 -input "path"

//     ./noemdoprograma -function 4 -input "/root/dump.txt" -offset 10 -function 2 -output "/usr/src/releasetools/text.txt" -input "ABCDEFGHIJKLMNOPOQESG" -dataSize 1000


#include <stdio.h>

typedef enum {
    FunctionOpen = 1,
    FunctionWrite,
    FunctionRead,
    FunctionSeek,
    FunctionClose
} Function;


void detectFunction(Function function);

int main(int argc, char *argv[]) {
    int count = 0;
    int argumentCount = argc;
    
    while (argumentCount > 0) { // Loop until there are no more arguments
        char *input;
        char *output;
        char *data;
        off_t offset;
        ssize_t dataSize;
        
        
        
        while (count < argc) {
            if (<#condition#>) {
                <#statements#>
            }
        }
        
        argumentCount--; // one argument was read, decrement it
    }
    return 1;
}

char *getArgument() {
    
}

void detectFunction(Function function) {
    switch (function) {
    case FunctionOpen:
        break;
    case FunctionWrite:
        break;
    case FunctionRead:
        break;
    case FunctionSeek:
        break;
    case FunctionClose:
        break;
    }
}

// Pegar o proximo argumento
// Checar se o argumento é uma funcao
// se for uma funcao
//      executar a função anterior
//      entrar no switch case daquela funcao
//      ler o proximo argumento
// Se não for uma funcão
//      atribuir este valor lido na variável correta
// Ler o proximo argumento
//
//
