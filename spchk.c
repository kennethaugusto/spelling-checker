#include "dict_handler.h"
#include "file_handler.h"
#include "processFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#ifndef DT_DIR 
#define DT_DIR 4
#define DT_REG 8
#endif 

int checkWord(dictionary_t *dictionary, wordEntry_t *entry, char *fileName){
	int start = 0;
	int length = strlen( entry -> word );

	for( int i = 0; i < length; i++ ){
		if( entry -> word[ i ] == '-' && start != -1 ){
			if( isInDictionary( dictionary , entry -> word + start , i - start  ) ){
				printf("File processed: %s\n", fileName);
                printf("Word '%s' in row %d and column %d found in dictionary\n", entry->word, entry->row, entry->column);
				return EXIT_FAILURE; 
			}
			start = -1;
		} else if( entry -> word[ i ] != '-' && start == -1 ){ // the entry does not have a '-'
			start = i;
		}
	}
	if( isInDictionary( dictionary , entry -> word + start , length - start ) ){
		printf("File processed: %s\n", fileName);
        printf("Word '%s' in row %d and column %d found in dictionary\n", entry->word, entry->row, entry->column);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int checkFile(dictionary_t *dictionary, char *fileName){
    filedata_t *parsedFile = processFile(fileName);
    if(parsedFile == NULL){
        fprintf(stderr, "Error: Could not parse file %s\n", fileName);
        return EXIT_FAILURE;
    }
    int status = EXIT_SUCCESS;
    for(wordEntry_t *entry = parsedFile->words; entry; entry = entry->next){
        status |= checkWord(dictionary, entry, fileName);
    }
    return status;
}

int checkDirectory(dictionary_t *dict, char *dir){
    char buffer[2000];
    DIR *dp = opendir(dir);
    struct dirent *entry;
    if(dp == NULL){
        perror("Error: Could not open directory");
        return EXIT_FAILURE;
    }
    while((entry = readdir(dp))){
        if(entry->d_name[0] == '.'){
            continue;
        }
        printf("Reading file: %s\n", entry->d_name);
        sprintf(buffer, "%s/%s", dir, entry->d_name);
        if(entry->d_type == DT_DIR){
            checkDirectory(dict, buffer);
        } else if(entry->d_type == DT_REG){
            size_t len = strlen(entry->d_name);
            if(len >= 4 && memcmp(entry->d_name + len - 4, ".txt", 4) == 0){
                checkFile(dict, buffer);
            }
        }
    }
    closedir(dp);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv){
    if(argc < 2){
        fprintf(stderr, "Please specify dictionary\n");
        return 1;
    }
    dictionary_t *dictionary;
    dictionary = readDictionary(argv[1]);
    if(dictionary == NULL){
        fprintf(stderr, "Could not read dictionary\n");
        return EXIT_FAILURE;
    }
    int error = EXIT_SUCCESS;
    for(int i = 2; i < argc; i++){
        if(checkDirectory(dictionary, argv[i]) != EXIT_SUCCESS){
            error = EXIT_FAILURE;
        }
    }
    return error;
}
