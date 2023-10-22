#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "configuration.h"
#include "cpu.h"
#include "read_file.h"

void initialise_window();
void process_input(void);
void update(void);
void render(void);
void destroy_window(void);
void handle_opcode(uint8_t* memory_address);

SDL_Window *window;
SDL_Renderer *renderer;

bool quit = false;
double latest_frame_time;

chip_8_machine chip_8;

int main(int argc, char *argv[])
{
  chip_8.game_start_address = load_program_file("IBM Logo.ch8");
  chip_8.pc_counter = 0;

  assign_program_memory(&chip_8);
  assign_font_set(&chip_8);

  handle_opcode(chip_8.game_start_address);

  initialise_window();

  while(!quit){
    process_input();
    update();
    render();
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
    0
  );
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
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

void update(void){
  double time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks64() - latest_frame_time);
  if(time_to_wait > 0 && time_to_wait < FRAME_TARGET_TIME){
    SDL_Delay(time_to_wait);
  }
  //Do updates here
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
    uint8_t next_four_bits = (*memory_address) & 0xFF;
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
    uint8_t variable_number = (*memory_address) & 0xF;
    uint8_t value = *(memory_address + 1);
    chip_8.variable_registers[variable_number] += value;
  break;

  case 'A':
    uint8_t next_four_bits = (*memory_address) & 0xFF;
    uint8_t last_eight_bits = *(memory_address + 1);
    chip_8.index_register = next_four_bits + last_eight_bits;
  break;
  
  case 'D':
    display_sprite(memory_address);
  break;
  }
}

void display_sprite(uint8_t* memory_address){
  uint8_t* x_coord_register = chip_8.variable_registers[(*memory_address) & 0xFF];
  uint8_t* y_coord_register = chip_8.variable_registers[*(memory_address + 1) & 0xFF00];

  uint8_t x_coord = *x_coord_register % DISPLAY_WIDTH;
  uint8_t y_coord = *y_coord_register % DISPLAY_HEIGHT;

  chip_8.variable_registers[15] = 0;
  
  uint8_t height = *(memory_address + 1) & 0xFF;

  SDL_Rect pixel = {0,0,1,1};
  for (int row = 0; row < height; row++)
  {
    if (y_coord + row >= DISPLAY_HEIGHT){
      break;
    }
    uint8_t sprite_data = *(chip_8.game_start_address + chip_8.index_register + row);
    //get data for every sprite that is coloured
    for (int bit = 0; bit < 8; bit++)
    {
      if(x_coord + bit >= DISPLAY_WIDTH || (sprite_data & bit) != bit){
        break;
      }

      pixel.x = x_coord;
      pixel.y = y_coord;

      uint8_t pixel_info;
      SDL_RenderReadPixels(renderer, &pixel, SDL_PIXELFORMAT_INDEX8, pixel_info, 1);

      //TODO: find if pixel has been turned on, not sure what if statement needs to be
      if (pixel_info)
      {
        chip_8.variable_registers[15] = 1;
        //Make pixel white
      }else{
        //turn pixel on
      }
    }
  }
}