#pragma once

#include <stdint.h>

extern char (*file_location)[];

void store_file_location(char (*file_location_input)[]);

uint8_t read_two_bytes_demo();
uint8_t read_two_bytes();