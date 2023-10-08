#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>

#include "configuration.h"
#include "cpu.h"

void initialise_window();
void process_input(void);
void update(void);
void render(void);
void destroy_window(void);

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

int main(int argc, char *argv[])
{
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