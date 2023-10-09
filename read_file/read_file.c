#include <stdio.h>

#include "read_file.h"

char (*file_location)[];
FILE *file = NULL;

void store_file_location(char (*file_location_input)[]){
    file_location = file_location_input;
}

uint8_t read_two_bytes(){
    file = fopen(file_location, "r+b");
    fclose(file);
}

uint8_t read_two_bytes_demo(){
    file = fopen("../games/IBM Logo.ch8", "r+b");
    unsigned char buffer[50];
    //fread(buffer,sizeof(buffer),1,file);
    buffer[0] = 1;
    fread(buffer,sizeof(buffer[0]),2,file);

    fclose(file);
}