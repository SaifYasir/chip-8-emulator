#include <stdint.h>
#include <stdlib.h>

#include "cpu.h"

chip_8_machine* assign_program_memory(chip_8_machine* chip_8){
    chip_8->chip_8_memory = calloc(MEMORY_SIZE_BYTES,sizeof(uint8_t));
    return chip_8;
}

chip_8_machine* assign_font_set(chip_8_machine* chip_8){
    uint8_t FONT_SET[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    int font_size = sizeof(FONT_SET);
    for (int i = 0; i < font_size; i++)
    {
        chip_8->chip_8_memory[FONT_ADDRESS_START + i] = FONT_SET[i];
    }
    return chip_8;
}

int remove_program_memory(chip_8_machine* chip_8){
    free(chip_8->chip_8_memory);
}


stack_t* add_to_stack(chip_8_machine* chip_8, uint16_t value){
    stack_t* stack = chip_8->stack;
    if(stack == NULL){
        chip_8->stack = malloc(sizeof(stack_t));
        chip_8->stack->value = value;
        chip_8->stack->next_ptr = NULL;
        return chip_8->stack;
    }
    while(stack->next_ptr != NULL){
        stack = stack->next_ptr;
    }
    stack_t* new_node = malloc(sizeof(stack_t));
    new_node->value = value;
    new_node->next_ptr = NULL;
    stack->next_ptr = new_node;
    return stack;
}

uint16_t pop_stack(chip_8_machine* chip_8){
    stack_t* stack = chip_8->stack;
    stack_t* previous_ptr = stack;
    if(stack == NULL){
        return NULL;
    }
    while(stack->next_ptr != NULL){
        previous_ptr = stack;
        stack = stack->next_ptr;
    }
    previous_ptr->next_ptr = NULL;
    uint16_t val = stack->value;
    if(previous_ptr == stack){
        chip_8->stack = NULL;
    }
    free(stack);
    return val;
}