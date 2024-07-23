#pragma once

typedef struct wordEntry_t {
	char *word;
	int row;
	int column;
	struct wordEntry_t *next;
} wordEntry_t;

typedef struct filedata_t {
	wordEntry_t *end;
	wordEntry_t *words;
	int currentRow;
	char *fileName;
} filedata_t;

filedata_t *processFile(char *fileName);