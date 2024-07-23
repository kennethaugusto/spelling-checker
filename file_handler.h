#pragma once
#include "dict_handler.h"
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#define BUFFERLENGTH 2048

typedef struct cursor_t {
	int fd;
	char buffer[ BUFFERLENGTH ];
	ssize_t end;
	ssize_t cursor;
} cursor_t;

cursor_t *openFile(char *fileName);
ssize_t readLine(cursor_t *cursor, char *buffer, size_t bufferLength);
int checkDirectory(dictionary_t *, char *path);
void closeFile(cursor_t *cursor);
