#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "read_file.h"

uint8_t* load_program_file(char *program_file_name){
    char prepend_path[] = "../games/";
    char full_path[strlen(program_file_name) + strlen(prepend_path) + 1];
    strcpy(full_path,prepend_path);
    strcat(full_path,program_file_name);
    FILE *file = fopen(full_path,"r+b");

    fseek(file,0,SEEK_END);
    long fsize = ftell(file);
    fseek(file,0,SEEK_SET);

    int instruction_amount = fsize/sizeof(uint8_t);
    uint8_t *game = calloc(instruction_amount,sizeof(uint8_t));

    fread(game, sizeof(uint8_t), instruction_amount, file);

    fclose(file);
    return game;
}

chip_8_machine* load_program_file_in_to_program_memory(chip_8_machine* chip_8, char *program_file_name){
    char prepend_path[] = "../games/";
    char full_path[strlen(program_file_name) + strlen(prepend_path) + 1];
    strcpy(full_path,prepend_path);
    strcat(full_path,program_file_name);
    FILE *file = fopen(full_path,"r+b");

    fseek(file,0,SEEK_END);
    long fsize = ftell(file);
    fseek(file,0,SEEK_SET);
    int instruction_amount = fsize/sizeof(uint8_t);

    chip_8->game_start_address = chip_8->chip_8_memory + ROM_ADDRESS_START;
    chip_8->pc_counter_end = ROM_ADDRESS_START + instruction_amount - 1;
    fread(chip_8->game_start_address, sizeof(uint8_t), instruction_amount, file);

    fclose(file);
    return chip_8;
}
