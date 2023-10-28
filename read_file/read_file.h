#pragma once

#include <stdint.h>
#include <stdio.h>

#include "cpu.h"

uint8_t* load_program_file(char *program_file_name);

chip_8_machine* load_program_file_in_to_program_memory(chip_8_machine* chip_8, char *program_file_name);