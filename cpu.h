#pragma once

#include <stdint.h>

#include "read_file.h"

#define MEMORY_SIZE_BYTES 4096
#define ROM_ADDRESS_START 0x200
#define FONT_ADDRESS_START 0x50

typedef struct chip_8_machine{
  uint8_t *chip_8_memory;

  int pc_counter;
  uint8_t* game_start_address;

  uint8_t delay_timer;
  uint8_t sound_timer;

  uint16_t index_register;

  uint8_t variable_registers[16];
} chip_8_machine;

chip_8_machine* assign_program_memory(chip_8_machine* chip_8);
chip_8_machine* assign_font_set(chip_8_machine* chip_8);