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
// -function 1 -path fileName -type read/write

// Write
// -function 2 -input "lorem ipsum" -dataSize 1000

// Read
// -function 3 -dataSize 1000

// Seek
// -function 4 -offset 123

// Close
// -function 5

//     ./noemdoprograma -function 4 -input "/root/dump.txt" -offset 10 -function 2 -output "/usr/src/releasetools/text.txt" -input "ABCDEFGHIJKLMNOPOQESG" -dataSize 1000

/**
 le o proximo argumento
 se for open, switch case no open, e le os args (arquivo novo/existente e a path)
 le o proximo argumento
 ve qual funcao que é (read, write, seek, close)
 entre no switch case da função. Dentro da função, chame OPEN.
 Caso a função seja write e o open em arquivo novo, modificar a flag pra ser um arquivo velho.
 execute a funcao (read, write, seek)
 Feche o descritor
 */



#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FILE_NAME_MAX_SIZE 30

typedef enum {
    FunctionOpen = 1,
    FunctionWrite,
    FunctionRead,
    FunctionSeek,
    FunctionClose
} Function;

int openFile(char *fileName, bool shouldCreate);

int main(int argc, char *argv[]) {
    int argumentPosition = 1;
    int argumentCount = argc;
    
    if (strcmp(argv[1], "open") != 0) {
        printf("O primeiro argumento deve abir um arquivo.");
        return -1;
    }
    
    bool isCreatingFile = true;
    char fileName[1][FILE_NAME_MAX_SIZE];
    int descriptor = -1;
    int offset = 0; // Offset used in the seek
    
    while (argumentCount > 0) { // Loop until there are no more arguments
        Function function = 0;
        printf("uma funcao\n");
        if (strcmp(argv[argumentPosition], "open") == 0) { function = FunctionOpen; }
        else if (strcmp(argv[argumentPosition], "write") == 0) { function = FunctionWrite; }
        else if (strcmp(argv[argumentPosition], "read") == 0) { function = FunctionRead; }
        else if (strcmp(argv[argumentPosition], "seek") == 0) { function = FunctionSeek; }
        else if (strcmp(argv[argumentPosition], "close") == 0) { function = FunctionClose; }
        else {
            //it's an argument
        }
        
        switch (function) {
            case FunctionOpen: {
                if ((strcmp(argv[argumentPosition + 1], "criar") == 0) || (strcmp(argv[argumentPosition + 1], "new") == 0)) {
                    isCreatingFile = true;
                } else if ((strcmp(argv[argumentPosition + 1], "abrir") == 0) || (strcmp(argv[argumentPosition + 1], "old") == 0)) {
                    isCreatingFile = false;
                } else {
                    printf("Argumento da funcao open invalido. Entre com 'criar' ou 'abrir'. Abortando.\n");
                    exit(-1);
                }
                strcpy(fileName[0], argv[argumentPosition + 2]);
                argumentPosition += 3;
                break;
            }
            case FunctionWrite: {
                descriptor = openFile(fileName[0], isCreatingFile);
                lseek(descriptor, offset, SEEK_SET);
                size_t bytesToWrite = atoi(argv[argumentPosition + 2]);
                size_t bytesWritten = write(descriptor, argv[argumentPosition + 1], bytesToWrite);
                printf("String to write: %s\nBytes to write: %zd\nBytes written: %zd\n", argv[argumentPosition + 1], bytesToWrite, bytesWritten);
                isCreatingFile = false;
                argumentPosition += 3;
                break;
            }
            case FunctionRead: {
                descriptor = openFile(fileName[0], isCreatingFile);
                lseek(descriptor, offset, SEEK_SET);
                size_t bytesToRead = atoi(argv[argumentPosition + 1]);
                size_t totalBytesRead = 0;
//                printf("Bytes to read: %zd", bytesToRead);
                printf("Conteudo do arquivo: ");
                while (totalBytesRead <= bytesToRead) {
                    char *buffer = malloc(bytesToRead);
                    size_t bytesRead = read(descriptor, buffer, bytesToRead);
                    printf("%s", buffer);
                    totalBytesRead += bytesRead;
                    bytesToRead -= bytesRead;
                }
                printf("\n");
                argumentPosition += 2;
                break;
            }
            case FunctionSeek: {
                descriptor = openFile(fileName[0], isCreatingFile);
                offset = atoi(argv[argumentPosition + 1]);
                lseek(descriptor, offset, SEEK_SET); // Constant defined in stdio
                
                argumentPosition += 2;
                break;
            }
            case FunctionClose: {
                argumentPosition++;
                break;
            }
        }
        close(descriptor);
        printf("argCount: %d, arg pos: %d\n", argumentCount, argumentPosition);
        argumentCount -= argumentPosition; // argumentCount = argumentCount - number of arguments read + 1(inc)
    }
    return 1;
}

/// Open Helper Functions
int openFile(char *fileName, bool shouldCreate) {
    int descriptor = -1;
    if (shouldCreate) {
        descriptor = open(fileName, O_RDWR | O_CREAT | O_EXCL);
    } else {
        descriptor = open(fileName, O_RDWR);
    }
    if (descriptor == -1) {
        fprintf(stderr, "Error opening file at path: %s\nError: %s\n", fileName, strerror(errno));
        exit(errno);
    }
    return descriptor;
}


//#ifdef DEBUG
//        printf("filePath: %s", fileName);
//        strcat(fileName, "/Users/rogerluan/Documents/Projects/SO-A/experimento7/exp7/");
//        printf("filePath: %s", fileName);
//#endif

// Pegar o proximo argumento
// Checar se o argumento é uma funcao
// se for uma funcao
//      executar a função anterior
//      entrar no switch case daquela funcao
//      ler o proximo argumento
// Se não for uma funcão
//      atribuir este valor lido na variável correta
// Ler o proximo argumento


//  ALEX VENTURINI  15294739
//  BRUNO PEDROSO   12662136
//  LUAN BONOMI     15108780
//  PEDRO CATALINI  15248354
//  ROGER OBA       12048534

// open novo_ou_velho nome_do_arquivo
// write o_que_for_digitado quantidade_de_caracteres_em_int
// seek quantidade_de_posicao_que_deve_dar_seek
// read quantidade_de_caracteres_que_vai_ler
// close (soh close)

// O_WRONLY SOMENTE PARA ESCRITA
// O_RDONLY SOMENTE PARA LEITURA
// O_RDWR ESCRITA E LEITURA

// argv[1] tem q ser open para abrir o arquivo!

/*

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int open_func(char *velho_novo, char *nome_arq, int qual) {
    int i = 0;
    char compara[1][10];
    int d = 0;
    
    strcpy(compara[0], velho_novo);
    
    if (strcmp(compara[0], "novo") == 0) {
        printf("novo\n");
        printf("qual = %d\n", qual);
        if (qual == 1) {
            printf("write\n");
            // ativa a funcao write
            d = open (nome_arq, O_WRONLY | O_CREAT);
            return d;
        } else {
            if (qual == 2) {
                printf("read\n");
                // ativa a funcao read
                d = open (nome_arq, O_RDONLY | O_CREAT);
                return d;
            } else {
                if(qual == 3){
                    printf("write e read\n");
                    d = open (nome_arq, O_RDWR | O_CREAT);
                    return d;
                }else{
                    // ativa nada e da erro
                    return -1;
                }
            }
        }
    } else {
        if (strcmp(compara[0], "velho") == 0) {
            printf("velho\n");
            if (qual == 1) {
                printf("write\n");
                // ativa a funcao write
                d = open(nome_arq, O_WRONLY);
                return d;
            } else {
                if (qual == 2) {
                    printf("read\n");
                    // ativa a funcao read
                    d = open(nome_arq, O_RDONLY);
                    return d;
                } else {
                    if (qual == 3) {
                        printf("write e read\n");
                        d = open(nome_arq, O_RDWR);
                        return d;
                    } else {
                        // ativa nada e da erro
                        return -1;
                    }
                }
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int i = 1, j = 0, k = 0;
    int d = 0;
    bool writeFlag = false;
    bool readFlag = false;
    char leu[100];
    if (strcmp(argv[i], "open") == 0) {
        i++;
        
        // TODO: rever logica de identificacao da funcao open se é read ou write
        while (i < argc) {
            if (strcmp(argv[i], "write") == 0) {
                writeFlag = true;
            } else if (strcmp(argv[i], "read") == 0) {
                readFlag = true;
            }
            i++;
        }
        
        if (writeFlag && readFlag) {
            d = open_func(argv[2], argv[3], 3);
        } else {
            if (writeFlag && !readFlag) {
                d = open_func(argv[2], argv[3], 1);
            } else if (!writeFlag && readFlag) {
                d = open_func(argv[2], argv[3], 2);
            }
        }
        i = 3;
        while(i < argc) {
            //write
            if (strcmp(argv[i], "write") == 0) {
                write(d, argv[i + 2], atoi(argv[i + 1]));
                i++;
            } else {
                //read
                if(strcmp(argv[i], "read") == 0) {
                    
                    read(d, leu, atoi(argv[i + 1]));
                    k = atoi(argv[i + 1]);
                    for(j = 0; j < k; j++){
                        printf("%c", leu[j]);
                    }
                    printf("\n");
                    i++;
                }
                else {
                    // Seek
                    if (strcmp(argv[i], "seek") == 0) {
                        // Falta a funcao seek
                        i++;
                    } else {
                        // Close
                        if (strcmp(argv[i], "close") == 0) {
                            i++;
                        } else {
                            i++;
                        }
                    }
                }
            }
        }
    } else {
        if (d == 0) {
            printf("Erro ao abrir o arquivo\n");
        }
        printf("Impossivel de abrir o arquivo!\n");
    }
    close(d);
}
*/