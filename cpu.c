#include <stdint.h>
#include <stdlib.h>

#include "cpu.h"

chip_8_machine* assign_program_memory(chip_8_machine* chip_8){
    chip_8->chip_8_memory_start = malloc(MEMORY_SIZE_BYTES * sizeof(uint8_t) + 1);
    return chip_8;
}

int remove_program_memory(chip_8_machine* chip_8){
    free(chip_8->chip_8_memory_start);
}