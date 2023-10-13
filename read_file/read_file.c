#include <stdio.h>
#include <stdlib.h>

#include "read_file.h"


uint8_t* read_two_bytes(FILE* file, int* two_byte_buffer){
    fread(two_byte_buffer, sizeof(uint8_t), 2, file);

    uint8_t val1 = *two_byte_buffer;
    uint8_t val2 = *(two_byte_buffer + 1);
}

FILE*  get_program_file(char *program_file_name){
    FILE *file = fopen("../games/IBM Logo.ch8","r+b");
    return file;
}

int close_program_file(FILE* file){
    return fclose(file);
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
