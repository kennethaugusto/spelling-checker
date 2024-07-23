#include "dict_handler.h"
#include "file_handler.h"
#include <fcntl.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void freeDictionary(dictionary_t *dictionary) {
    if (dictionary->table != NULL) {
        free(dictionary->table);
    }
    free(dictionary);
}

int compareTwoStrings(char *string1, char *string2, int length1, int length2) {
    for(int i = 0; i < length1 && i < length2; i++) {
        int capstring1 = toupper(string1[i]);
        int capstring2 = toupper(string2[i]);
        if (capstring1 != capstring2) {
            return capstring1 - capstring2;
        }
    }
    return length1 - length2;
}

int isInDictionary(dictionary_t *dictionary, char *word, int length) {
    ssize_t index = findIndexInDictionary(dictionary, word, length);
    if (index == dictionary->length) {
        return EXIT_FAILURE;
    }
    char *entry = dictionary->table[index];
    if (strlen(entry) != length) {
        return EXIT_FAILURE;
    }
    if (islower(word[0])) {
        for (int i = 0; i < length; i++) {
            if (isupper(entry[i]) || isupper(word[i])) {
                return EXIT_FAILURE;
            }
            if (entry[i] != word[i]) {
                return EXIT_FAILURE;
            }
        }
    } else {
        int mustBeUpper = 0;
        int allUpper = 1;
        for (int i = 0; i < length; i++) {
            if (isupper(entry[i]) && !isupper(word[i])) {
                return EXIT_FAILURE;
            }
            if (isupper(word[i]) && !isupper(entry[i]) && i != 0) {
                mustBeUpper = 1;
            }
            if (islower(word[i])) {
                allUpper = 0;
            }
            if (toupper(word[i]) != toupper(entry[i])) {
                return EXIT_FAILURE;
            }
        }
        if (mustBeUpper && !allUpper) { 
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

ssize_t findIndexInDictionary(dictionary_t *dictionary, char *word, int length) {
    ssize_t min = 0;
    ssize_t max = dictionary->length;
    while (max - min >= 1) {
        ssize_t index = (max + min) / 2;
        int result = compareTwoStrings(word, dictionary->table[index], length, strlen(dictionary->table[index]));
        if (result == 0) {
            return index;
        } else if (result < 0) {
            max = index;
        } else if (result > 0) {
            min = index + 1;
        }
    }
    return min;
}

int insertIntoDictionary(dictionary_t *dict, char *word, size_t wordLength) {
    if (word[wordLength - 1] == '\n') {
        word[wordLength - 1] = '\0';
        wordLength -= 1;
    } 
    if (dict->length == dict->capacity) {
        dict->capacity *= 2;
        char **newTable = realloc(dict->table, dict->capacity);
        if (newTable == NULL) {
            fprintf(stderr, "Error: Table cannot be resized\n");
            dict->capacity = dict->length;
            return EXIT_FAILURE;
        }
        dict->table = newTable;
    }
    ssize_t index = findIndexInDictionary(dict, word, wordLength);
    memmove(dict->table + index + 1, dict->table + index, sizeof(char *) * (dict->length - index));

    dict->table[index] = malloc(wordLength + 1);
    if (dict->table[index] == NULL) {
        fprintf(stderr, "Error: Word not allocated\n");
        return EXIT_FAILURE;
    }
    memcpy(dict->table[index], word, wordLength);
    dict->table[index][wordLength] = '\0';
    dict->length++;
    return EXIT_SUCCESS;
}

dictionary_t *readDictionary(char *fileName) {
    cursor_t *cursor = openFile(fileName);
    if (cursor == NULL) {
        return NULL;
    }
    dictionary_t *dict = malloc(sizeof(dictionary_t));
    if (dict == NULL) {
		fprintf( stderr , "Error: Dictionary cannot be allocated\n" );
        closeFile(cursor);
        return NULL;
    }
    dict->capacity = 1024;
    dict->length = 0;
    dict->table = malloc(sizeof(char *) * dict->capacity);
    if (dict->table == NULL) {
        fprintf(stderr, "Error: Dictionary table cannot be allocated\n");
        free(dict);
        closeFile(cursor);
        return NULL;
    }
    char buffer[BUFFERLENGTH];
    ssize_t result;
    do {
        result = readLine(cursor, buffer, BUFFERLENGTH);
        if (result > 0) {
            if (insertIntoDictionary(dict, buffer, result)) {
                fprintf(stderr, "Error: Word cannot be inserted into dictionary\n");
                freeDictionary(dict);
                closeFile(cursor);
                return NULL;
            }
        }
    } while (result > 0);
    if (result < 0) {
        fprintf(stderr, "Error: Could not read file\n");
        freeDictionary(dict);
        closeFile(cursor);
        return NULL;
    }
    closeFile(cursor);
    return dict;
}