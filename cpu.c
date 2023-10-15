#include "stdint.h"
#include "stdlib.h"

#include "cpu.h"

uint8_t* assign_program_memory(chip_8_machine* chip_8){
    chip_8->chip_8_memory = malloc(MEMORY_SIZE_BYTES * sizeof(uint8_t));
}

int remove_program_memory(chip_8_machine* chip_8){
    free(chip_8->chip_8_memory);
}