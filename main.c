#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "configuration.h"
#include "cpu.h"
#include "read_file.h"

void initialise_window();
void process_input(void);
void delay_time(void);
void render(void);
void destroy_window(void);
void handle_opcode(uint8_t* memory_address);
void display_sprite(uint8_t* memory_address);

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *back_buffer;

bool quit = false;
double last_frame_time;
double timer_miliseconds_remainder = 0;
uint8_t last_key_pressed = UINT8_MAX;

chip_8_machine chip_8;

int main(int argc, char *argv[])
{
  assign_program_memory(&chip_8);
  load_program_file_in_to_program_memory(&chip_8,"1dcell.ch8");
  assign_font_set(&chip_8);
  initialise_window();

  while (chip_8.pc_counter < chip_8.pc_counter_end && !quit)
  {
    handle_opcode(chip_8.chip_8_memory + chip_8.pc_counter);
    chip_8.pc_counter+=2;
    process_input();
  }

  while (!quit)
  {
    delay_time();
    process_input();
  }
  

  destroy_window();
  return 0;
}

void initialise_window(void){
  SDL_Init(SDL_INIT_EVERYTHING);
  window = SDL_CreateWindow(
    "CHIP-8",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    EMULATOR_SCREEN_WIDTH,
    EMULATOR_SCREEN_HEIGHT,
    SDL_WINDOW_RESIZABLE
  );
  renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_RenderSetLogicalSize(renderer, DISPLAY_WIDTH, DISPLAY_HEIGHT);
  back_buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,DISPLAY_WIDTH,DISPLAY_HEIGHT);
}

void process_input(void){
  SDL_Event event;

  while(SDL_PollEvent(&event) != false){
    if(event.type == SDL_QUIT){
      quit = true;
      return;
    }
    else if (event.type == SDL_WINDOWEVENT){
      if(event.window.event == SDL_WINDOWEVENT_RESIZED){
        SDL_RenderCopy(renderer, back_buffer, NULL, NULL);
        SDL_RenderPresent(renderer);
      }
    }
    else if (event.type == SDL_KEYDOWN)
    {
      switch (event.key.keysym.sym)
      {
        case SDLK_ESCAPE:
          quit = true;
        break;

        case SDLK_1:
          last_key_pressed = 1;
        break;
        case SDLK_2:
          last_key_pressed = 2;
        break;
        case SDLK_3:
          last_key_pressed = 3;
        break;
        case SDLK_4:
          last_key_pressed = 0xC;
        break;
        case SDLK_q:
          last_key_pressed = 4;
        break;
        case SDLK_w:
          last_key_pressed = 5;
        break;
        case SDLK_e:
          last_key_pressed = 6;
        break;
        case SDLK_r:
          last_key_pressed = 0xD;
        break;
        case SDLK_a:
          last_key_pressed = 7;
        break;
        case SDLK_s:
          last_key_pressed = 8;
        break;
        case SDLK_d:
          last_key_pressed = 9;
        break;
        case SDLK_f:
          last_key_pressed = 0xE;
        break;
        case SDLK_z:
          last_key_pressed = 0xA;
        break;
        case SDLK_x:
          last_key_pressed = 0;
        break;
        case SDLK_c:
          last_key_pressed = 0xB;
        break;
        case SDLK_v:
          last_key_pressed = 0xF;
        break;
      }
    }
  }
}


void delay_time(void){
  double time_passed = SDL_GetTicks64() - last_frame_time;
  timer_miliseconds_remainder += fmod(time_passed ,TIME_PER_CYCLE_MILLISECONDS);
  uint8_t total_cycle_ammount = (time_passed / TIME_PER_CYCLE_MILLISECONDS) + (timer_miliseconds_remainder / TIME_PER_CYCLE_MILLISECONDS);
  if(chip_8.delay_timer > 0){
    chip_8.delay_timer = chip_8.delay_timer < total_cycle_ammount ? 0 : chip_8.delay_timer - total_cycle_ammount;
  }
  if(chip_8.sound_timer > 0){
    chip_8.sound_timer = chip_8.sound_timer < total_cycle_ammount ? 0 : chip_8.sound_timer - total_cycle_ammount;
  }
  timer_miliseconds_remainder = fmod(timer_miliseconds_remainder,TIME_PER_CYCLE_MILLISECONDS);
  double time_to_wait = SECS_PER_INSTRUCTION - time_passed;
  if(time_to_wait > 0 && time_to_wait < SECS_PER_INSTRUCTION){
    SDL_Delay(time_to_wait);
  }
  last_frame_time = SDL_GetTicks64();
}

void render(void){
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
}

void destroy_window(void){
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void handle_opcode(uint8_t* memory_address){
  delay_time();

  uint8_t most_significant_hex = *memory_address >> 4;
  uint8_t second_most_significant_hex = *memory_address & 0xF;
  uint8_t third_most_significant_hex = *(memory_address + 1) >> 4;
  uint8_t fourth_most_significant_hex = *(memory_address + 1) & 0xF;

  switch (most_significant_hex)
  {

  case 0x0:
    //Check second byte to see if its  00E0, 00EE or 0NNN
    switch ((third_most_significant_hex << 4) + fourth_most_significant_hex)
    {
    case 0xE0:
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      SDL_SetRenderTarget(renderer,back_buffer);
      SDL_RenderClear(renderer);
      SDL_SetRenderTarget(renderer, NULL);
      SDL_RenderCopy(renderer, back_buffer, NULL, NULL);
      SDL_RenderPresent(renderer);
      break;
    
    case 0xEE:
      //Main loop will increment pc counter by 2, skipping the jmp / call instruction
      chip_8.pc_counter = pop_stack(&chip_8);
      break;

    //Assume command is calling Machine Code Routine at 0NNN, this is not implemented
    default:
      break;
    }
  break;

  //OPCODE 1NNN jump to NNN address
  case 0x1:
    uint16_t jump_counter = (second_most_significant_hex << 8) + (third_most_significant_hex << 4) + fourth_most_significant_hex;
    
    //Make pc counter 2 less than expected due to main loop incrementing pc counter by 2
    chip_8.pc_counter = jump_counter - 2;
  break;

  case 0x2:
    uint16_t call_counter = (second_most_significant_hex << 8) + (third_most_significant_hex << 4) + fourth_most_significant_hex;
    add_to_stack(&chip_8,chip_8.pc_counter);

    //Make pc counter 2 less than expected due to main loop incrementing pc counter by 2
    chip_8.pc_counter = call_counter - 2;
  break;
  
  case 0x3:
    if(chip_8.variable_registers[second_most_significant_hex] == (third_most_significant_hex << 4) + fourth_most_significant_hex){
      chip_8.pc_counter += 2;
    }
  break;

  case 0x4:
    if(chip_8.variable_registers[second_most_significant_hex] != (third_most_significant_hex << 4) + fourth_most_significant_hex){
      chip_8.pc_counter += 2;
    }
  break;

  case 0x5:
    if(chip_8.variable_registers[second_most_significant_hex] == chip_8.variable_registers[third_most_significant_hex]){
      chip_8.pc_counter += 2;
    }
  break;

  case 0x6:
    chip_8.variable_registers[second_most_significant_hex] = (third_most_significant_hex << 4) + fourth_most_significant_hex;
  break;

  case 0x7:
    chip_8.variable_registers[second_most_significant_hex] += (third_most_significant_hex << 4) + fourth_most_significant_hex;
  break;

  case 0x8:
    switch (fourth_most_significant_hex)
    {
      case 0:
        chip_8.variable_registers[second_most_significant_hex] = chip_8.variable_registers[third_most_significant_hex];
      break;

      case 1:
        chip_8.variable_registers[second_most_significant_hex] = chip_8.variable_registers[second_most_significant_hex] | chip_8.variable_registers[third_most_significant_hex];
      break;

      case 2:
        chip_8.variable_registers[second_most_significant_hex] = chip_8.variable_registers[second_most_significant_hex] & chip_8.variable_registers[third_most_significant_hex];
      break;

      case 3:
        chip_8.variable_registers[second_most_significant_hex] = chip_8.variable_registers[second_most_significant_hex] ^ chip_8.variable_registers[third_most_significant_hex];
      break;

      case 4:
        if(chip_8.variable_registers[second_most_significant_hex] + chip_8.variable_registers[third_most_significant_hex] > UINT8_MAX)
        {
          chip_8.variable_registers[second_most_significant_hex] += chip_8.variable_registers[third_most_significant_hex];
          chip_8.variable_registers[0xF] = 1;
        }else{
          chip_8.variable_registers[second_most_significant_hex] += chip_8.variable_registers[third_most_significant_hex];
          chip_8.variable_registers[0xF] = 0;
        }
      break;

      case 5:
        if(chip_8.variable_registers[second_most_significant_hex] - chip_8.variable_registers[third_most_significant_hex] < 0)
        {
          chip_8.variable_registers[second_most_significant_hex] -= chip_8.variable_registers[third_most_significant_hex];
          chip_8.variable_registers[0xF] = 0;
        }else{
          chip_8.variable_registers[second_most_significant_hex] -= chip_8.variable_registers[third_most_significant_hex];
          chip_8.variable_registers[0xF] = 1;
        }
      break;

      case 6:
        uint8_t shifted_right_bit = chip_8.variable_registers[second_most_significant_hex] & 1;
        chip_8.variable_registers[second_most_significant_hex] >>= 1;
        chip_8.variable_registers[0xF] = shifted_right_bit;
      break;

      //NOT TESTED
      case 7:
        if(chip_8.variable_registers[third_most_significant_hex] - chip_8.variable_registers[second_most_significant_hex] < 0)
        {
          chip_8.variable_registers[second_most_significant_hex] = chip_8.variable_registers[third_most_significant_hex] - chip_8.variable_registers[second_most_significant_hex];
          chip_8.variable_registers[0xF] = 0;
        }else{
          chip_8.variable_registers[second_most_significant_hex] = chip_8.variable_registers[third_most_significant_hex] - chip_8.variable_registers[second_most_significant_hex];
          chip_8.variable_registers[0xF] = 1;
        }
      break;

      case 0xE:
        uint8_t shifted_left_bit = chip_8.variable_registers[second_most_significant_hex] & 0x80 ? 1:0;
        chip_8.variable_registers[second_most_significant_hex] <<= 1;
        chip_8.variable_registers[0xF] = shifted_left_bit;
      break;
    }
  break;

  case 0x9:
    if(chip_8.variable_registers[second_most_significant_hex] != chip_8.variable_registers[third_most_significant_hex]){
      chip_8.pc_counter += 2;
    }
  break;

  case 0xA:
    chip_8.index_register = (second_most_significant_hex << 8) + (third_most_significant_hex << 4) + fourth_most_significant_hex;
  break;

  //NOT TESTED
  case 0xB:
    uint16_t jump_counter_flow = (second_most_significant_hex << 8) + (third_most_significant_hex << 4) + fourth_most_significant_hex;
    
    //Make pc counter 2 less than expected due to main loop incrementing pc counter by 2
    chip_8.pc_counter = jump_counter_flow + chip_8.variable_registers[0] - 2;
  break;

  case 0xC:
    chip_8.variable_registers[second_most_significant_hex] = (rand() % 255) & ((third_most_significant_hex << 4) + fourth_most_significant_hex);
  break;
  
  case 0xD:
    display_sprite(memory_address);
  break;

  case 0xE:
    switch (fourth_most_significant_hex)
    {
      //NOT TESTED
      case 0x1:
        if (last_key_pressed != chip_8.variable_registers[second_most_significant_hex])
        {
          chip_8.pc_counter += 2;
        }
      break;
    
      //NOT TESTED
      case 0xE:
        if(last_key_pressed == chip_8.variable_registers[second_most_significant_hex]){
          chip_8.pc_counter += 2;
          last_key_pressed = UINT8_MAX;
        }
      break;
    }
  break;

  case 0xF:
    switch ((third_most_significant_hex << 4) + fourth_most_significant_hex)
    {
      //NOT TESTED
      case 0x07:
        chip_8.variable_registers[second_most_significant_hex] = chip_8.delay_timer;
      break;

      //TODO: IMPLEMENT
      case 0x0A:
        while (last_key_pressed > 0xF)
        {
          process_input();
        }      
      break;

      case 0x15:
        chip_8.delay_timer = chip_8.variable_registers[second_most_significant_hex];
      break;

      //NOT TESTED
      case 0x18:
        chip_8.sound_timer = chip_8.variable_registers[second_most_significant_hex];
      break;

      case 0x1E:
        chip_8.index_register += chip_8.variable_registers[second_most_significant_hex];
      break;

      case 0x29:
        chip_8.index_register = FONT_ADDRESS_START + (chip_8.variable_registers[second_most_significant_hex] * 5);
      break;

      case 0x33:
        uint8_t val = chip_8.variable_registers[second_most_significant_hex];
        *(chip_8.chip_8_memory + chip_8.index_register) = val / 100;
        *(chip_8.chip_8_memory + chip_8.index_register + 1) = (val % 100) / 10;
        *(chip_8.chip_8_memory + chip_8.index_register + 2) = (val % 10);
      break;

      case 0x55:
        for (int i = 0; i <= second_most_significant_hex; i++)
        {
          *(chip_8.chip_8_memory + chip_8.index_register + i) = chip_8.variable_registers[i];
        }
      break;

      case 0x65:
        for (int i = 0; i <= second_most_significant_hex; i++)
        {
          chip_8.variable_registers[i] = *(chip_8.chip_8_memory + chip_8.index_register + i);
        }
      break;
    }
  break;
  }
}

void display_sprite(uint8_t* memory_address){
  uint8_t x_coord_register = chip_8.variable_registers[(*memory_address) & 0xF];
  uint8_t y_coord_register = chip_8.variable_registers[*(memory_address + 1) >> 4];

  uint8_t x_coord = x_coord_register % DISPLAY_WIDTH;
  uint8_t y_coord = y_coord_register % DISPLAY_HEIGHT;

  chip_8.variable_registers[15] = 0;
  
  uint8_t height = *(memory_address + 1) & 0xF;

  SDL_Rect pixel = {0,0,1,1};
  uint32_t pixel_info;
  for (int row = 0; row < height; row++)
  {
    if (y_coord + row >= DISPLAY_HEIGHT){
      break;
    }
    SDL_SetRenderTarget(renderer,back_buffer);
    uint8_t sprite_data = *(chip_8.chip_8_memory + chip_8.index_register + row);

    //get data for every sprite that is coloured
    for (int bit = 0; bit < 8; bit++)
    {
      uint8_t bit_value = (int)pow(2,7-bit);
      bool bit_value_is_1 = ((sprite_data & bit_value) == bit_value);

      if(x_coord + bit >= DISPLAY_WIDTH || !bit_value_is_1){
        continue;
      }

      pixel.x = x_coord + bit;
      pixel.y = y_coord + row;

      //SDL_PIXELFORMAT_INDEX8
      SDL_RenderReadPixels(renderer, &pixel, 0, &pixel_info, 1);

      //Because format is ARGB; to see if its not black needs to be greater than 1 byte (Opacity is 8 bits)
      if (pixel_info != 0 && pixel_info != 0xFF000000)
      {
        chip_8.variable_registers[15] = 1;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawPoint(renderer,pixel.x,pixel.y);
        //Make pixel white
      }else{
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawPoint(renderer,pixel.x,pixel.y);
      }
    }
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, back_buffer, NULL, NULL);
    SDL_RenderPresent(renderer);
  }
}