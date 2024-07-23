#include "file_handler.h"
#include <fcntl.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

cursor_t *openFile(char *fileName) {
    cursor_t *cursor = malloc(sizeof(cursor_t));
    if (cursor == NULL) {
        fprintf(stderr, "Error: Unable to allocate file cursor\n");
        return NULL;
    }
    cursor->fd = open(fileName, O_RDONLY, 0);
    cursor->end = 0;
    cursor->cursor = 0;
    if (cursor->fd == -1) {
        perror("Error: Could not open file");
        free(cursor);
        return NULL;
    }
    return cursor;
}

ssize_t readLine(cursor_t *cursor, char *buffer, size_t length) {
    ssize_t j = 0;
    ssize_t result;
    do {
        for (ssize_t i = cursor->cursor; i < cursor->end && j < length; i++, j++) {
            buffer[j] = cursor->buffer[i];
            if (cursor->buffer[i] == '\n') {
                cursor->cursor = i + 1;
                return j;
            }
        }
        if (j >= length) {
            return -1;
        }
        result = read(cursor->fd, cursor->buffer, BUFFERLENGTH);
        if (result > 0) {
            cursor->cursor = 0;
            cursor->end = result;
        }
    } while (result > 0);
    if (result < 0) {
        perror("Unable to read file");
        return -1;
    }
    return j;
}

void closeFile(cursor_t *cursor) {
    close(cursor->fd);
    free(cursor);
}
