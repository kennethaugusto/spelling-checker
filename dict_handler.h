#pragma once
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

typedef struct dictionary_t{
	char **table;
	size_t length;
	size_t capacity;
} dictionary_t;

dictionary_t *readDictionary(char *fileName);
ssize_t findIndexInDictionary(dictionary_t *dict, char *word, int length);
int isInDictionary(dictionary_t *dict, char *word, int length);