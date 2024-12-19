#ifndef MAIN_H
#define MAIN_H

#define STATUS_STATE_LIVES 0
#define STATUS_STATE_GAME 1
#define STATUS_STATE_GAMEOVER 2

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

typedef struct
{
    float x, y; // posizione del player
    float dx, dy; // delta posizione, velocita
    short lives;
    char* name;
    int onLedge, isDead; // flags

    int animFrame, facingRight, slowingDown;
} Man;

typedef struct {
    int x, y;
} Crystal;

typedef struct {
    int x, y, w, h;
} Ledge;

typedef struct
{
    float scrollX;

    //Players
    Man man;

    //Crystals
    Crystal crystals[100];

    //Ledges
    Ledge ledges[100];

    //Images
    SDL_Texture* crystal;
    SDL_Texture* menFrames[2];
    SDL_Texture* brick;
    SDL_Texture* label;
    int labelW, labelH;

    //Fonts
    TTF_Font* font;

    int time;
    int statusState;

    //Renderer
    SDL_Renderer* renderer;
} GameState;

#endif // MAIN_H
