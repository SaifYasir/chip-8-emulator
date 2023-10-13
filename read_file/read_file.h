#pragma once

#include <stdint.h>
#include <stdio.h>

uint8_t read_program_demo();

FILE*  get_program_file(char *program_file_name);
int close_program_file(FILE* file);

uint8_t* read_two_bytes(FILE* file, int* two_byte_buffer);