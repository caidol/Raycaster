#include "raycaster.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define mapWidth        24
#define screenWidth     1024
#define screenHeight    768

uint8_t worldMap[mapWidth][mapWidth]=
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

Vector2 getCurrentRayDirection(Player *player, int currentX){
    /*
    calculate the current x-coordinate in camera space 

        -> -1 is left side of screen
        ->  0 is centre of screen 
        ->  1 is right side of screen
    */

   float cameraX = 2 * (currentX / screenWidth) - 1;
   
   // calculate the ray direction
   float rayDirX = player->dir.x + (player->plane.x * cameraX);
   float rayDirY = player->dir.x + (player->plane.y * cameraX);

   return (Vector2) {.x = rayDirX, .y = rayDirY};
}

int main(void){
    // initialise SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        fprintf("Failed to initialise SDL2: %s\n", SDL_GetError());
        exit(1);
    }

    //TODO: Create an assert macro to automate error checking on return values of initialisation 

    struct game_state *gameState = NULL;
    // initalise SDL2 window and renderer states
    gameState->window = SDL_CreateWindow(
        "Wolfenstein 3D style raycaster",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight, 
        SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (gameState->window == NULL){
        fprintf("Failed to create SDL2 window: %s\n", SDL_GetError());
        exit(1);
    }

    gameState->renderer = SDL_CreateRenderer(
        gameState->window, 
        -1,
        SDL_RENDERER_PRESENTVSYNC
    );

    if (gameState->renderer == NULL){
        fprintf("Failed to create SDL2 renderer: %s\n", SDL_GetError());
        exit(1);
    }

    // Initialise player struct

    Player player = {
        .pos = {22, 12}, 
        .dir = {-1, 0}, 
        .plane = {0, 0.66},
    };

    /*
        the current map box we're in
    
        -> represented as the integer value of our real position to approximate location on MAP array
    */
    Vector2 mapSquare = {
        .x = (int) player.pos.x,
        .y = (int) player.pos.y
    };

    return 0;
}
