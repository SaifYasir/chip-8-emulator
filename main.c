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

bool quit = false;
double latest_frame_time;

chip_8_machine chip_8;

int main(int argc, char *argv[])
{
  assign_program_memory(&chip_8);
  load_program_file_in_to_program_memory(&chip_8,"IBM Logo.ch8");
  assign_font_set(&chip_8);

  chip_8.pc_counter = 0;
  initialise_window();

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

//NEED TO STOP WHEN PROGRAM ENDS
  while (chip_8.pc_counter < chip_8.pc_counter_end)
  {
    handle_opcode(chip_8.game_start_address + chip_8.pc_counter);
    chip_8.pc_counter+=2;
  }

  while (true)
  {
    SDL_Delay(UINT32_MAX);
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
    DISPLAY_WIDTH,
    DISPLAY_HEIGHT,
    SDL_WINDOW_RESIZABLE
  );
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  SDL_RenderSetLogicalSize(renderer, DISPLAY_WIDTH, DISPLAY_HEIGHT);
}

void process_input(void){
  SDL_Event event;

  while(SDL_PollEvent(&event) != false){
    if(event.type == SDL_QUIT){
      quit = true;
      return;
    }
    else if (event.type == SDL_KEYDOWN)
    {
      switch (event.key.keysym.sym)
      {
      case SDLK_ESCAPE:
        quit = true;
        break;
      }
    }
    
  }
}

void delay_time(void){
  double time_to_wait = SECS_PER_INSTRUCTION - (SDL_GetTicks64() - latest_frame_time);
  if(time_to_wait > 0 && time_to_wait < SECS_PER_INSTRUCTION){
    SDL_Delay(time_to_wait);
  }
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

  char most_significant_hex = (*(memory_address) >> 4) + '0';

  //If opcode is past '9' in ASCII, move to ASCII 'A'
  most_significant_hex = most_significant_hex > '9' ? most_significant_hex - ':' + 'A'  : most_significant_hex;

  switch (most_significant_hex)
  {

  case '0':
    //Check second byte to see if its  00E0, 00EE or 0NNN
    switch (*(memory_address + 1))
    {
    case 0xE0:
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      SDL_RenderClear(renderer);
      break;
    
    case 0xEE:
      /* code */
      break;

    //Assume command is calling Machine Code Routine at 0NNN
    default:
      break;
    }
    break;

  //OPCODE 1NNN jump to NNN address
  case '1':
    uint8_t next_four_bits = (*memory_address) & 0xF;
    uint8_t last_eight_bits = *(memory_address + 1);
    uint16_t jmp_address = next_four_bits + last_eight_bits;
    handle_opcode(chip_8.chip_8_memory + jmp_address);
    break;

  case '6':
    uint8_t variable_number = (*memory_address) & 0xF;
    uint8_t value = *(memory_address + 1);
    chip_8.variable_registers[variable_number] = value;
  break;

  case '7':
    uint8_t variable_number_7 = (*memory_address) & 0xF;
    uint8_t value_7 = *(memory_address + 1);
    chip_8.variable_registers[variable_number_7] += value_7;
  break;

  case 'A':
    uint16_t next_four_bits_A = ((*memory_address) & 0xF) << 8;
    uint16_t last_eight_bits_A = *(memory_address + 1);
    chip_8.index_register = next_four_bits_A + last_eight_bits_A;
  break;
  
  case 'D':
    display_sprite(memory_address);
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
    uint8_t sprite_data = *(chip_8.chip_8_memory + chip_8.index_register + row);

    //get data for every sprite that is coloured
    for (int bit = 0; bit < 8; bit++)
    {
      uint8_t bit_value = (int)pow(2,7-bit);
      bool bit_value_is_1 = (sprite_data & bit_value) == bit_value;

      if(x_coord + bit >= DISPLAY_WIDTH || !bit_value_is_1){
        continue;
      }

      pixel.x = x_coord + bit;
      pixel.y = y_coord + row;

      //SDL_PIXELFORMAT_INDEX8
      SDL_RenderReadPixels(renderer, &pixel, 0, &pixel_info, 1);

      //Because format is ARGB; to see if its not black needs to be greater than 1 byte (Opacity is 8 bits)
      if (pixel_info > 0)
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
    SDL_RenderPresent(renderer);
  }
}