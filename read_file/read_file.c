#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "read_file.h"


uint8_t* read_two_bytes(FILE* file, uint8_t* two_byte_buffer){
    fread(two_byte_buffer, sizeof(uint8_t), 2, file);

    int val1 = *two_byte_buffer;
    int val2 = *(two_byte_buffer + 1);
}

FILE*  get_program_file(char *program_file_name){
    char prepend_path[] = "../games/";
    char full_path[strlen(program_file_name) + strlen(prepend_path) + 1];
    strcpy(full_path,prepend_path);
    strcat(full_path,program_file_name);
    FILE *file = fopen(full_path,"r+b");
    return file;
}

int close_program_file(FILE* file){
    return fclose(file);
}

uint8_t* load_program_file(char *program_file_name){
    char prepend_path[] = "../games/";
    char full_path[strlen(program_file_name) + strlen(prepend_path) + 1];
    strcpy(full_path,prepend_path);
    strcat(full_path,program_file_name);
    FILE *file = fopen(full_path,"r+b");

    fseek(file,0,SEEK_END);
    long fsize = ftell(file);
    fseek(file,0,SEEK_SET);

    uint8_t *game = malloc(fsize + 1);
    int instruction_amount = fsize/sizeof(uint8_t);

    fread(game, sizeof(uint8_t), instruction_amount, file);

    fclose(file);
    return game;
}

uint8_t read_program_demo(){
    FILE *file = fopen("../games/IBM Logo.ch8", "r+b");

    fseek(file,0,SEEK_END);
    long fsize = ftell(file);
    unsigned char buffer[50];
    fseek(file,0,SEEK_SET);

    uint8_t *game = malloc(fsize + 1);
    fread(game, fsize, 1, file);

    fclose(file);
}
