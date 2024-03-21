#include "raycaster.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

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
  {1,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,3,0,3,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,3,0,0,0,0,3,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,3,0,3,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,3,0,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool quit;
} State;

static State gameState = {
    .quit = false,
};

static int currentVerLine = 0;

Vector2 getCurrentRayDirection(Player *player, int currentX){
    /*
    calculate the current x-coordinate in camera space 

        -> -1 is left side of screen
        ->  0 is centre of screen 
        ->  1 is right side of screen
    */

   float cameraX = 2 * currentX / (float) screenWidth - 1;
   
   // calculate the ray direction
   float rayDirX = player->dir.x + (player->plane.x * cameraX);
   float rayDirY = player->dir.y + (player->plane.y * cameraX);

   return (Vector2) {.x = rayDirX, .y = rayDirY};
}

/* 
calculate the change in distance either for a ray as it traverses one along in x-direction
or one along in y-direction.
*/
Vector2 calculateDeltaDistances(Vector2 *rayDir){
    // set to arbritrarily high number if zero
    float deltaDistX = (rayDir->x == 0) ? 1e30 : fabsf(sqrt(1 + (rayDir->y * rayDir->y) / (rayDir->x * rayDir->x)));
    float deltaDistY = (rayDir->y == 0) ? 1e30 : fabsf(sqrt(1 + (rayDir->x * rayDir->x) / (rayDir->y * rayDir->y)));
    
    return (Vector2) {.x = deltaDistX, .y = deltaDistY};
}

Vector2I findMapSteps(Vector2 *rayDir){
    int stepX;
    int stepY;
    
    if (rayDir->x < 0){ // the ray vector points to left side of screen
        stepX = -1;
    }
    else{
        stepX = 1;
    }

    if (rayDir->y < 0){
        stepY = -1;
    }
    else{
        stepY = 1;
    }

    return (Vector2I) {.x = stepX, .y = stepY};
}

/* calculate the rays initial intersection with either a horizontal or vertical line */
Vector2 extendRayToFirstHit(Vector2 *deltaDist, Vector2I *mapSquare, Vector2 *rayDir, Player *player){
    // calculate the nearest X and Y distances as a
    // proportion of the delta X and Y through a square

    float nearestDistX;
    float nearestDistY;

    if (rayDir->x < 0){ // the ray vector points to left side of screen
        nearestDistX = (player->pos.x - mapSquare->x) * deltaDist->x;
    }
    else{
        nearestDistX = (mapSquare->x + 1.0f - player->pos.x) * deltaDist->x;
    }

    if (rayDir->y < 0){
        nearestDistY = (player->pos.y - mapSquare->y) * deltaDist->y;
    }
    else{
        nearestDistY = (mapSquare->y + 1.0f - player->pos.y) * deltaDist->y;
    }

    return (Vector2) {.x = nearestDistX, .y = nearestDistY};
}

void drawLineToScreen(Vector2 *deltaDist, Vector2 *nearestDist, Vector2I *mapSquare, int side){
    colourRGBA colour;
    // check map square to determine colour
    switch (worldMap[mapSquare->y][mapSquare->x]){
        case 1:
            colour = red;
            break;
        case 2:
            colour = green;
            break;
        case 3:
            colour = blue; 
            break;
    }

    float perpWallDist;
    switch(side){
        case 0: // east or west
            perpWallDist = (nearestDist->x - deltaDist->x);
            break;
        case 1:
            perpWallDist = (nearestDist->y - deltaDist->y);

            colour.r /= 2;
            colour.g /= 2;
            colour.b /= 2;
            break;
    }

    int line_height = (int) (screenHeight / perpWallDist);

    // determine the start and end positions to draw line at
    int line_start = -line_height / 2 + screenHeight / 2;
    if (line_start < 0) {line_start = 0;}

    int line_end = line_height / 2 + screenHeight / 2;
    if (line_end >= screenHeight) {line_end = screenHeight - 1;}

    SDL_SetRenderDrawColor(gameState.renderer, colour.r, colour.g, colour.b, colour.a);
    SDL_RenderDrawLine(gameState.renderer, currentVerLine, line_start, currentVerLine, line_end);
}

/* extend the ray out from the player's position, checking until a wall (non-zero) has been hit */
void raycast(Vector2 *deltaDist, Vector2 *nearestDist, Vector2I *mapSquare, Vector2I *stepDir, Player *player){
    // Here is where we properly perform the DDA (Digital Differential Analyzer) algorithm

    int side;

    bool hit = false;
    while (!hit){
        // check for which of the nearest distances intersects first and update the other one
        if (nearestDist->x < nearestDist->y){
            nearestDist->x += deltaDist->x;
            mapSquare->x += stepDir->x;

            side = 0;
        }
        else{
            nearestDist->y += deltaDist->y;
            mapSquare->y += stepDir->y;

            side = 1;
        }

        // check current map square of ray
        if (worldMap[mapSquare->y][mapSquare->x] > 0){
            hit = true;
        }
    }

    drawLineToScreen(deltaDist, nearestDist, mapSquare, side);
}

void render(Player player){
    for (currentVerLine = 0; currentVerLine < screenWidth; currentVerLine++){
        Vector2 rayDir = getCurrentRayDirection(&player, currentVerLine);

        /*
        the current map box we're in
    
        -> represented as the integer value of our real position to approximate location on worldMap array
        */
        Vector2I mapSquare = {
            .x = (int) player.pos.x,
            .y = (int) player.pos.y
        };

        Vector2 deltaDist = calculateDeltaDistances(&rayDir);

        Vector2 nearestDist = extendRayToFirstHit(&deltaDist, &mapSquare, &rayDir, &player);
        Vector2I stepDir = findMapSteps(&rayDir);
        

        // perform raycast 
        raycast(&deltaDist, &nearestDist, &mapSquare, &stepDir, &player);

    }

    currentVerLine = 0;
}

int main(int argc, char *argv[]){
    // initialise SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("Failed to initialise SDL2: %s\n", SDL_GetError());
        exit(1);
    }

    //TODO: Create an assert macro to automate error checking on return values of initialisation 

    // initalise SDL2 window and renderer states
    gameState.window = SDL_CreateWindow(
        "Wolfenstein 3D style raycaster",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight, 
        0
    );

    if (!gameState.window){
        printf("Failed to create SDL2 window: %s\n", SDL_GetError());
        exit(1);
    }

    gameState.renderer = SDL_CreateRenderer(
        gameState.window, 
        -1,
        SDL_RENDERER_PRESENTVSYNC
    );

    if (!gameState.renderer){
        printf("Failed to create SDL2 renderer: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1");
    SDL_SetRelativeMouseMode(true);

    // Initialise player struct
    Player player = {
        .pos = {.x = 22.0f, .y = 12.0f}, 
        .dir = {.x = -1.0f, .y = 0.0f}, 
        .plane = {.x = 0.0f, .y = 0.66f},
    };

    const float rotateSpeed = 0.025;
    const float moveSpeed = 0.05;

    gameState.quit = false;

    while (!gameState.quit){
        SDL_Event event;
        int mouse_xrel = 0;

        while(SDL_PollEvent(&event)){
            switch (event.type){
                case SDL_QUIT:
                    gameState.quit = true;
                    break;
                case SDL_MOUSEMOTION:
                    mouse_xrel = event.motion.xrel; 
                    break;
            }
        }

        const uint8_t *keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_ESCAPE]) {gameState.quit = true;}

        // Apply this rotation matrix below
        //
        //  -> [cos x,  -sin x]
        //  -> [sin x,   cos x]
        //

        if (mouse_xrel != 0){ // rotate player
            float rotSpeed = rotateSpeed * (mouse_xrel * -0.1);

            // both camera direction and camera plane must be rotated

            Vector2 oldDir = player.dir;    
            player.dir.x = player.dir.x * cosf(rotSpeed) - player.dir.y * sinf(rotSpeed);
            player.dir.y = oldDir.x     * sinf(rotSpeed) + player.dir.y * cosf(rotSpeed);

            Vector2 oldPlane = player.plane;
            player.plane.x = player.plane.x * cosf(rotSpeed) - player.plane.y * sinf(rotSpeed);
            player.plane.y = oldPlane.x     * sinf(rotSpeed) + player.plane.y * cosf(rotSpeed);
        }

        Vector2 deltaPos = {.x = player.dir.x * moveSpeed, .y = player.dir.y * moveSpeed};

        if (keystate[SDL_SCANCODE_W]){ // forwards
            if (worldMap[(int) player.pos.y][(int) (player.pos.x + deltaPos.x)] == 0){
                player.pos.x += deltaPos.x;
            }

            if (worldMap[(int) (player.pos.y + deltaPos.y)][(int) player.pos.x] == 0){
                player.pos.y += deltaPos.y;
            }
        }
        if (keystate[SDL_SCANCODE_S]){ // backwards
            if (worldMap[(int) player.pos.y][(int) (player.pos.x - deltaPos.x)] == 0){
                player.pos.x -= deltaPos.x;
            }

            if (worldMap[(int) (player.pos.y - deltaPos.y)][(int) player.pos.x] == 0){
                player.pos.y -= deltaPos.y;
            }
        }
        if (keystate[SDL_SCANCODE_A]){ // strafe left
            if (worldMap[(int) player.pos.y][(int) (player.pos.x - deltaPos.y)] == 0){
                player.pos.x -= deltaPos.y;
            }

            if (worldMap[(int) (player.pos.y - -deltaPos.x)][(int) player.pos.x] == 0){
                player.pos.y += deltaPos.x;
            }
        }
        if (keystate[SDL_SCANCODE_D]){ // strafe right
            if (worldMap[(int) player.pos.y][(int) (player.pos.x + deltaPos.y)] == 0){
                player.pos.x += deltaPos.y;
            }

            if (worldMap[(int) (player.pos.y - deltaPos.x)][(int) player.pos.x] == 0){
                player.pos.y -= deltaPos.x;
            }
        }

        SDL_SetRenderDrawColor(gameState.renderer, 0x18, 0x18, 0x18, 0xFF);
        SDL_RenderClear(gameState.renderer);

        render(player);

        SDL_RenderPresent(gameState.renderer);
    }

    SDL_DestroyRenderer(gameState.renderer);
    SDL_DestroyWindow(gameState.window);
    return 0;
}
