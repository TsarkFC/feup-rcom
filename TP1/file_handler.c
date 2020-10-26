#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "file_handler.h"

//remove
#include "data_stuffing.h"

char* read_file(char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (file == NULL){
        perror("Error reading file");
    }

    char* line = malloc(1024);
    char* buffer = malloc(1024);

    while (fread(line , sizeof(*line), 10, file) > 0){
        int new_size = strlen(line) + strlen(buffer);
        if (strlen(line) + strlen(buffer) > sizeof(buffer)){
            buffer = realloc(buffer, new_size);
        }

        strncat(buffer, line, strlen(line));
        memset(line, 0, 1024);
    }

    free(line);
    fclose(file);
    return buffer;
}

int write_file(char* file_path, char* buffer) {
    FILE* file = fopen(file_path, "w");
    if (file == NULL){
        perror("Error reading file");
    }
    
    if (fputs(buffer, file) < 0){
        printf("Error writing to file!\n");
    }
    fclose(file);
    return 0;
}

//USED FOR TESTING REASONS

/*
char** divideBuffer(char* buffer, int* size) {
	int position = -1, pos = 0;

    char** divided_data;

	for (int i = 0; i < strlen(buffer); i++){
		pos = i % 255;
		if (pos == 0){
			position++;
			divided_data = realloc(divided_data, (position + 1) * sizeof(char*));
			divided_data[position] = (char*)calloc(255, sizeof(char));
		}
		divided_data[position][pos] = buffer[i];
	}
    *size = position + 1;
	return divided_data;
}

int main(){
    char* buffer = read_file("test.txt");
    int num_of_divisions = 0;
    char** divided_buffer = divideBuffer(buffer, &num_of_divisions);

    for (int i = 0; i < num_of_divisions; i++){
        data_stuff ds = stuffData(divided_buffer[i], strlen(divided_buffer[i]));
        if (i == num_of_divisions - 1){
            for (int j = 0; j < ds.data_size; j++){
                printf("%c ", ds.data[i]);
            }
            printf("\nStuffed:\n%s\n\n", ds.data);
            printf("\nBuffer:\n%s\n\n", divided_buffer[i]);
        }
    }
    
    //write_file("hmmm.txt", buffer);
    return 0;
}
*/