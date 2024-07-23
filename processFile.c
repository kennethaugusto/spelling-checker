#include "processFile.h"
#include "file_handler.h"
#include <fcntl.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

void freeEntry(wordEntry_t *entry){
    free(entry->word);
    free(entry);
}

void freeFileData(filedata_t *parsedFile){
    wordEntry_t *entry = parsedFile->words;
    while(entry != NULL){
        parsedFile->words = entry->next;
        freeEntry(entry);
        entry = parsedFile->words; 
    }
    free(parsedFile->fileName);
    free(parsedFile);
}

filedata_t *createFileData(char *fileName){
    filedata_t *parsedFile = malloc(sizeof(filedata_t));
    if(parsedFile == NULL){
        fprintf(stderr, "Error: Allocation failed\n");
        return NULL;
    }
    parsedFile->end = NULL;
    parsedFile->words = NULL;
    parsedFile->currentRow = 1;
    parsedFile->fileName = strdup(fileName);
    if(parsedFile->fileName == NULL){
        fprintf(stderr, "Allocation failed\n");
        free(parsedFile);
        return NULL;
    }
    return parsedFile;
}

int appendEntry(filedata_t *parsedFile, char *word, int length, int column){
    wordEntry_t *newEntry = malloc(sizeof(wordEntry_t));
    if(newEntry == NULL){
        fprintf(stderr, "Error: Unable to allocate parsed entry\n");
        return EXIT_FAILURE;
    }
    newEntry->word = malloc(length + 1);
    memcpy(newEntry->word, word, length);
    newEntry->word[length] = '\0';
    newEntry->row = parsedFile->currentRow;
    newEntry->column = column;
    newEntry->next = NULL;
    if(parsedFile->end != NULL){
        parsedFile->end->next = newEntry;
        parsedFile->end = newEntry;
    } else {
        parsedFile->end = newEntry;
        parsedFile->words = newEntry;
    }
    return EXIT_SUCCESS;
}

int processLine(char *buffer, int length, filedata_t *parsedFile) {
    int start = -1; // inside a word if start != -1
    for (int i = 0; i < length; i++) {
        char c = buffer[i];
        if (isalpha(c)) { // do not include punctuation in the word
            if (start == -1) {
                start = i;
            }
        } else if (c == '-' && start != -1) {
            continue;
        } else if ((isspace(c)) && start != -1) {
            if (appendEntry(parsedFile, buffer + start, i - start, start + 1)) {
                return EXIT_FAILURE;
            }
            start = -1;
        } else if ((isspace(c) || c == '{' || c == '[' || c == '(' || c == '\'' || c == '"' || isdigit(c)) && start == -1) {
            continue;
        } else if ((c == ',' || c == '.' || c == ';' || c == ':' || c == '!' || c == '+' || c == '#' || c == '$' || c == '*' || c == '%' || c == '/' || c == '@' || c == '=' || c == '<' || c == '>' || c == '~' || c == '^') && isalpha(buffer[i - 1]) && (i < length - 1 && isalpha(buffer[i + 1]))) {
            continue; // Ignore commas before or after a word	
        } else if ((c == '}' || c == ']' || c == ')' || c == '/' || c == '\\' || c == '"' || c == ',' || c == '.' || c == ';' || c == ':' || c == '!' || c == '+' || c == '#' || c == '$' || c == '*' || c == '%' || c == '/' || c == '@' || c == '=' || c == '<' || c == '>' || c == '~' || c == '^' || isdigit(c)) && isalpha(buffer[i - 1]) && !isalpha(buffer[i + 1])) {
            if (start != -1) {
                if (appendEntry(parsedFile, buffer + start, i - start, start + 1)) {
                    return EXIT_FAILURE;
                }
                start = -1;
            }
        }
    }
    if (start != -1) {
        if (appendEntry(parsedFile, buffer + start, length - start, start + 1)) {
            return EXIT_FAILURE;
	    }
    }
    parsedFile->currentRow++;
    return EXIT_SUCCESS;
    }

filedata_t *processFile(char *fileName){
    cursor_t *cursor = openFile(fileName);
    if(cursor == NULL){
        return NULL;
    }
    filedata_t *parsedFile = createFileData(fileName);
    if(parsedFile == NULL){
        closeFile(cursor);
        return NULL;
    }
    char buffer[BUFFERLENGTH];
    ssize_t result;

    do{
        result = readLine(cursor, buffer, BUFFERLENGTH);
        if(result > 0){
            if(processLine(buffer, result, parsedFile)){
                closeFile(cursor);
                freeFileData(parsedFile);
                return NULL;
            }
        }
    } while(result > 0);
    closeFile(cursor);
    
    if(result < 0){
        freeFileData(parsedFile);
        return NULL;
    }
    return parsedFile;
}