#include "..\..\sdl2\i686-w64-mingw32\include\SDL2\SDL.h"
#undef main

typedef struct{
    float x; float y;
} Vector2;

typedef struct{
    Vector2 pos;
    Vector2 dir;
    Vector2 plane;
} Player;

struct game_state{
    SDL_Window *window;
    SDL_Renderer *renderer;
};

Vector2 findRayDirection(Player *player, int currentX);
void findNextXStep();
void findNextYStep();
void extendRayToFirstHit();