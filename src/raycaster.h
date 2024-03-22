#include "..\..\sdl2\i686-w64-mingw32\include\SDL2\SDL.h"
#include "..\..\sdl2\i686-w64-mingw32\include\SDL2\SDL_events.h"
#include "..\..\sdl2\i686-w64-mingw32\include\SDL2\SDL_hints.h"
#include "..\..\sdl2\i686-w64-mingw32\include\SDL2\SDL_render.h"
#include "..\..\sdl2\i686-w64-mingw32\include\SDL2\SDL_mouse.h"
#include "..\..\sdl2\i686-w64-mingw32\include\SDL2\SDL_keyboard.h"
#include "..\..\sdl2\i686-w64-mingw32\include\SDL2\SDL_scancode.h"
#undef main

typedef struct{
    float x, y;
} Vector2;

typedef struct{
    int x, y;
} Vector2I;

typedef struct{
    Vector2 pos;
    Vector2 dir;
    Vector2 plane;
} Player;

/*
struct game_state{
    SDL_Window *window;
    SDL_Renderer *renderer;
};
*/

typedef struct{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} colourRGBA;

colourRGBA red = {.r = 0xFF, .g = 0x00, .b = 0x00, .a = 0xFF};
colourRGBA green = {.r = 0x00, .g = 0xFF, .b = 0x00, .a = 0xFF};
colourRGBA blue = {.r = 0x00, .g = 0x00, .b = 0xFF, .a = 0xFF};

//static struct game_state *gameState = NULL;
//static int currentVerLine;

Vector2 findRayDirection(Player *player, int currentX);
Vector2 extendRayToFirstHit(Vector2 *deltaDist, Vector2I *mapSquare, Vector2 *rayDir, Player *player);
Vector2I findMapSteps(Vector2 *rayDir);
Vector2 calculateDeltaDistances(Vector2 *rayDir);
void raycast();
void render();