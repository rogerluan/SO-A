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
// open [new/old] [fileName]

// Write
// write [dataToWrite] [numberOfBytesToWrite]

// Read
// read [numberOfBytesToRead]

// Seek
// seek [offset]

// Close
// close


#include <stdio.h>      // printf
#include <fcntl.h>      // open options
#include <stdbool.h>    // boolean type
#include <errno.h>      // errno constant
#include <stdlib.h>     // malloc
#include <string.h>     // string helper functions (strcmp, strcpy)
#include <unistd.h>     // lseek, open, etc
//#include <sys/stat.h>
//#include <sys/types.h>

#define FILE_NAME_MAX_SIZE 30

typedef enum {
    FunctionNull,
    FunctionOpen,
    FunctionWrite,
    FunctionRead,
    FunctionSeek,
    FunctionClose
} Function;

/**
 Returns an enum representing a function, based on the arguments sent.
 
 @param arg A char pointer containing a string that might represent a function. The function will be extracted from this argument.
 @return enum that identifies which function was read. Returns FunctionNull if the argument is not a valid function.
 */
Function getFunction(char *arg);

/**
 Checks if we're opening a new file.
 
 @param arg A char pointer that might represent a new file argument or not.
 Exits the program if the argument is invalid.
 Accepted strings: "criar" and "new" for new files, and "abrir" and "old" when opening old files.
 @return true if the user is creating a new file, otherwise, false.
 */
bool isNewFile(char *arg);

/**
 Opens the file based on the arguments.
 
 @param fileName Name of the file to be opened.
 @param shouldCreate Boolean indicating if it should create a new file or open an existing one.
 @return returns the descriptor created, if the file opening was successful. Otherwise, returns -1.
 */
int openFile(char *fileName, bool shouldCreate);

int main(int argc, char *argv[]) {
    int argumentPosition = 1;
    int argumentCount = argc - 1;
    int argumentsRead = 0;
    
    if (strcmp(argv[1], "open") != 0) {
        printf("O primeiro argumento deve abrir um arquivo. Abortando.\n\n\n");
        return -1;
    }
    
    bool isCreatingFile = true;
    char fileName[1][FILE_NAME_MAX_SIZE];
    int descriptor = -1;
    size_t lastSizeRead = 0;
    char *lastDataRead = malloc(lastSizeRead);
    
    while (argumentCount > 0) { // Loop until there are no more arguments
        Function function = getFunction(argv[argumentPosition]);
        
        switch (function) {
            case FunctionNull: break;
            case FunctionOpen: {
                printf("Executing open function.\n");
                isCreatingFile = isNewFile(argv[argumentPosition + 1]);
                strcpy(fileName[0], argv[argumentPosition + 2]);
                descriptor = openFile(fileName[0], isCreatingFile);
                argumentsRead = 3;
                argumentPosition += argumentsRead;
                break;
            }
            case FunctionWrite: {
                printf("Executing write function.\n");
                
                size_t bytesToWrite;
                char *buffer;

                if (argumentCount == 1 || (argumentCount > 1 && (getFunction(argv[argumentPosition + 1]) != FunctionNull))) { // There are no more parameters after write function
                    printf("Print last buffer.");
                    bytesToWrite = lastSizeRead;
                    buffer = lastDataRead;
                } else {
                    printf("Print content.");
                    bytesToWrite = atoi(argv[argumentPosition + 2]);
                    buffer = argv[argumentPosition + 1];
                }
                
                size_t totalBytesWritten = 0;
                size_t bytesWritten = 0;
                printf("Bytes a serem escritos: %zd.\n", bytesToWrite);
                while (totalBytesWritten <= bytesToWrite) {
                    bytesWritten = write(descriptor, buffer + bytesWritten, bytesToWrite);
                    totalBytesWritten += bytesWritten;
                    bytesToWrite -= bytesWritten;
                }
                printf("Conteudo escrito: %s\nTotal de bytes escritos: %zd\n", buffer, bytesWritten);
                isCreatingFile = false;
                argumentsRead = 3;
                argumentPosition += argumentsRead;
                break;
            }
            case FunctionRead: {
                printf("Executing read function.\n");
                size_t bytesToRead = atoi(argv[argumentPosition + 1]);
                size_t totalBytesRead = 0; // Total number of bytes read
                size_t bytesRead = 0;      // Bytes read in one operation
                printf("Bytes a serem lidos: %zd.\n", bytesToRead);
                char *buffer = malloc(bytesToRead);
                while (totalBytesRead <= bytesToRead) {
                    bytesRead = read(descriptor, buffer + bytesRead, bytesToRead);
                    totalBytesRead += bytesRead;
                    bytesToRead -= bytesRead;
                }
                printf("Conteudo do arquivo: %s\nTotal de bytes lidos: %zd\n", buffer, totalBytesRead);
                lastDataRead = buffer;
                lastSizeRead = totalBytesRead;
                printf("lastdataread: %s", lastDataRead);
                argumentsRead = 2;
                argumentPosition += argumentsRead;
                break;
            }
            case FunctionSeek: {
                printf("Executing seek function.\n");
                off_t offsetToSeek = atoi(argv[argumentPosition + 1]);
                off_t offsetSought = lseek(descriptor, offsetToSeek, SEEK_SET);
                printf("Offset para deslocar: %lld\nOffset deslocado:%lld\n", offsetToSeek, offsetSought);
                argumentsRead = 2;
                argumentPosition += argumentsRead;
                break;
            }
            case FunctionClose: {
                printf("Executing close function.\n");
                argumentsRead = 1;
                argumentPosition += argumentsRead;
                close(descriptor);
                break;
            }
        }
        argumentCount -= argumentsRead;
    }
    printf("\n\n");
    return 1;
}

/***************************************************/
//
//                  Helper Functions
//
/***************************************************/

Function getFunction(char *arg) {
    Function function = FunctionNull;
    if (strcmp(arg, "open") == 0) { function = FunctionOpen; }
    else if (strcmp(arg, "write") == 0) { function = FunctionWrite; }
    else if (strcmp(arg, "read") == 0) { function = FunctionRead; }
    else if (strcmp(arg, "seek") == 0) { function = FunctionSeek; }
    else if (strcmp(arg, "close") == 0) { function = FunctionClose; }
    return function;
}

bool isNewFile(char *arg) {
    if ((strcmp(arg, "criar") == 0) || (strcmp(arg, "new") == 0)) {
        return true;
    } else if ((strcmp(arg, "abrir") == 0) || (strcmp(arg, "old") == 0)) {
        return false;
    } else {
        printf("Argumento da funcao open invalido. Entre com 'criar' ou 'abrir'. Abortando.\n\n\n");
        exit(-1);
    }
}

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

void print(const char * __restrict fmt, ...) {
#ifdef DEBUG
//    printf("%s\n", fmt, ##__VA_ARGS__);
#endif
}

