#include <stdio.h>
#include "main.h"

void init_status_lives(GameState* game)
{
	char str[128] = "";

	sprintf_s(str, sizeof(str), "x %d", (int)game->man.lives);

	SDL_Color white = { 255,255,255,255 };

	SDL_Surface* tmp = TTF_RenderText_Blended(game->font, str, white);
	game->labelH = tmp->h;
	game->labelW = tmp->w;
	game->label = SDL_CreateTextureFromSurface(game->renderer, tmp);
	SDL_FreeSurface(tmp);
}
void draw_status_lives(GameState* game)
{
	// Set render color to black
	SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

	// Clear the screen
	SDL_RenderClear(game->renderer);

	SDL_Rect rect = { 320-70, 240-24,48,48 };
	SDL_RenderCopyEx(game->renderer, game->menFrames[0], NULL, &rect, 0, NULL, game->man.facingRight);

	// Set render color to black
	SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);

	SDL_Rect textRect = {320-20, 240-game->labelH/2, game->labelW, game->labelH};
	SDL_RenderCopy(game->renderer, game->label, NULL, &textRect);
}
void shutdown_status_lives(GameState* game)
{
	SDL_DestroyTexture(game->label);
	game->label = NULL;
}