#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>

typedef struct
{
	int x, y;
	short life;
	char* name;
} Man;

typedef struct {
	int x, y;
} Crystal;

typedef struct
{
	// Players
	Man man;

	// Crystals
	Crystal crystals[100];

	// Images
	SDL_Texture* crystal;
	SDL_Renderer* renderer;
} GameState;

void loadGame(GameState *game)
{
	SDL_Surface* crystalSurface = NULL;

	// Load images and create rendering textures from them
	crystalSurface = IMG_Load("crystal.png");
	if (crystalSurface == NULL)
	{
		printf("Cannot find crystal.png!\n\n");
		SDL_Quit();
		exit(1);
	}

	game->crystal = SDL_CreateTextureFromSurface(game->renderer, crystalSurface);		// carico la texture dalla surface nell'attributo
	SDL_FreeSurface(crystalSurface);												// libero la memoria occupata dalla surface

	game->man.x = 320 - 40;
	game->man.y = 240 - 40;

	// init crystals
	for (int i = 0; i < 100; i++)
	{
		game->crystals[i].x = rand()%640;
		game->crystals[i].y = rand()%480;
	}
}

int processEvents(SDL_Window* window, GameState* game)
{
	SDL_Event event;
	int done = 0;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_WINDOWEVENT_CLOSE:
		{
			if (window)
			{
				SDL_DestroyWindow(window);
				window = NULL;
				done = 1;
			}
		}
		break;
		case SDL_KEYDOWN:
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				done = 1;
				break;
			}
		}
		break;
		case SDL_QUIT:
			done = 1;
			break;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_LEFT])
	{
		game->man.x -= 10;
	}
	if (state[SDL_SCANCODE_RIGHT])
	{
		game->man.x += 10;
	}
	if (state[SDL_SCANCODE_UP])
	{
		game->man.y -= 10;
	}
	if (state[SDL_SCANCODE_DOWN])
	{
		game->man.y += 10;
	}
	return done;
}

void doRender(SDL_Renderer* renderer, GameState* game)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

	// Clear the screen (blue)
	SDL_RenderClear(renderer);

	// Set render color to white
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_Rect rect = { game->man.x,game->man.y,80,80 };
	SDL_RenderFillRect(renderer, &rect);

	//draw the crystal image
	for (int i = 0; i < 100; i++)
	{
		SDL_Rect crystalRect = { game->crystals[i].x,game->crystals[i].y,64,64};
		SDL_RenderCopy(renderer, game->crystal, NULL, &crystalRect);
	}

	// We are done drawing, "present" or show to the screen what we've drawn
	SDL_RenderPresent(renderer);
}

int main(int argc, char* args[]) {
	GameState gameState;
	SDL_Window* window = NULL;			// Declare a window
	SDL_Renderer* renderer = NULL;		// Declare a renderer

	SDL_Init(SDL_INIT_VIDEO);	// Initialize SDL2

	srand(time(0));

	// Create an application window with the following settings:
	window = SDL_CreateWindow(
		"Game Window",				// window tile
		SDL_WINDOWPOS_UNDEFINED,	// initial x position
		SDL_WINDOWPOS_UNDEFINED,	// initial y position
		640,						// width in pixel
		480,						// height in pixel
		0);							// flags

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	gameState.renderer = renderer;

	loadGame(&gameState);

	// The window is open: enter program loop (see SDL_PoolEvent)
	int done = 0;

	//Event loop
	while (!done)
	{
		// Check for events
		done = processEvents(window, &gameState);

		// Render display
		doRender(renderer, &gameState);

		// Don't burn up the CPU
		//SDL_Delay(10);
	}

	// Shutdown game and unload all memory
	SDL_DestroyTexture(gameState.crystal);

	// Close and destroy the window
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	// Clean up
	SDL_Quit();
	return 0;
}