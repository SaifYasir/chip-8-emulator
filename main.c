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
void handle_opcode(void);

uint8_t delay_timer;
uint8_t sound_timer;

uint8_t variable_registers[16];

unsigned short index_register;
unsigned short program_counter;

//unsure how to declare 4kB or RAM
//unsure how to declare "a stack of 16 bit addresses"

SDL_Window *window;
SDL_Renderer *renderer;

bool quit = false;
double latest_frame_time;

uint8_t* buffer = NULL;
FILE *game_file = NULL;

struct game{
  FILE *game_file;
  uint16_t pc_counter;
  uint16_t game_start_address;
} game;

int main(int argc, char *argv[])
{
  uint16_t val1 = 0x00E0;

  load_program_file("IBM Logo.ch8");
  game_file = get_program_file("IBM Logo.ch8");
  buffer = malloc(sizeof(uint8_t) * 2);
  read_two_bytes(game_file,buffer);
  handle_opcode();

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
  free(buffer);
  close_program_file(game_file);
}

void handle_opcode(void){
  char opcode = (int)(buffer[0] / 16) + '0';

  //If opcode is past '9' in ASCII, move to ASCII 'A'
  opcode = opcode > '9' ? opcode - ':' + 'A'  : opcode;

  switch (opcode)
  {

  case '0':
    //Check second byte to see if its  00E0, 00EE or 0NNN
    switch (buffer[1])
    {
    case 0xE0:
      SDL_RenderClear(renderer);
      break;
    
    case 0XEE:
      /* code */
      break;

    //Assume command is calling Machine Code Routine at 0NNN
    default:
      break;
    }
    break;

  //OPCODE 1NNN jump to NNN address
  case '1':
    uint16_t address = (int)(buffer[0] % 16) +  buffer[1];
   //Need to figure out how to jump to a certain address
    break;

  
  default:
    break;
  }
}