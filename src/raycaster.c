#include "raycaster.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

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

/* 
calculate the change in distance either for a ray as it traverses one along in x-direction
or one along in y-direction.
*/
void calculateDeltaDistances(Vector2 *rayDir){
    float deltaDistX = (rayDir->x == 0) 
        ? (rayDir->x = 999999) // set to arbritrarily high number
        : sqrt(1 + (rayDir->y * rayDir->y / rayDir->x * rayDir->x));

    float deltaDistY = (rayDir->y == 0)
        ? (rayDir->y = 999999)
        : sqrt(1 + (rayDir->x * rayDir->x / rayDir->y * rayDir->y));

    return (Vector2) {.x = deltaDistX, .y = deltaDistY};
}

/* calculate the rays initial intersection with either a horizontal or vertical line */
void extendRayToFirstHit(Vector2 *deltaDist, Vector2 *mapSquare, Vector2 *rayDir, Player *player){
    int x = 0;

    // calculate the nearest X and Y distances as a
    // proportion of the delta X and Y through a square

    float nearestDistX = 0;
    float nearestDistY = 0;

    if (rayDir->x < 0){ // the ray vector points to left side of screen
        float stepX = -1;
        nearestDistX = (player->pos.x - mapSquare->x) * deltaDist->x;
    }
    else{
        float stepX = 1;
        nearestDistX = (mapSquare->x + 1 - player->pos.x) * deltaDist->x;
    }

    if (rayDir->y < 0){
        float stepY = -1;
        nearestDistY = (player->pos.y - mapSquare->y) * deltaDist->y;
    }
    else{
        float stepY = 1;
        nearestDistY = (mapSquare->y + 1 - player->pos.y) * deltaDist->y;
    }

    return (Vector2) {.x = nearestDistX, .y = nearestDistY};
}

int main(int argc, char *argv[]){
    // initialise SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("Failed to initialise SDL2: %s\n", SDL_GetError());
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
        printf("Failed to create SDL2 window: %s\n", SDL_GetError());
        exit(1);
    }

    gameState->renderer = SDL_CreateRenderer(
        gameState->window, 
        -1,
        SDL_RENDERER_PRESENTVSYNC
    );

    if (gameState->renderer == NULL){
        printf("Failed to create SDL2 renderer: %s\n", SDL_GetError());
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
