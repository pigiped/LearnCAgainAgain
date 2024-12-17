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

typedef struct {
	int x, y, w, h;
} Ledge;

typedef struct
{
	// Players
	Man man;

	// Crystals
	Crystal crystals[100];

	// Ledges
	Ledge ledges[100];

	// Images
	SDL_Texture* crystal;
	SDL_Texture* menFrames[2];
	SDL_Texture* brick;

	// Renderer
	SDL_Renderer* renderer;
} GameState;

void loadGame(GameState *game)
{
	SDL_Surface* surface = NULL;

	// Load images and create rendering textures from them
	surface = IMG_Load("crystal.png");
	if (surface == NULL)
	{
		printf("Cannot find crystal.png!\n\n");
		SDL_Quit();
		exit(1);
	}

	game->crystal = SDL_CreateTextureFromSurface(game->renderer, surface);		// carico la texture dalla surface nell'attributo
	SDL_FreeSurface(surface);												// libero la memoria occupata dalla surface

	surface = IMG_Load("man_1.png");
	if (surface == NULL)
	{
		printf("Cannot find man_1.png!\n\n");
		SDL_Quit();
		exit(1);
	}

	game->menFrames[0] = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("man_2.png");
	if (surface == NULL)
	{
		printf("Cannot find man_2.png!\n\n");
		SDL_Quit();
		exit(1);
	}

	game->menFrames[1] = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("brick.png");
	if (surface == NULL)
	{
		printf("Cannot find brick.png!\n\n");
		SDL_Quit();
		exit(1);
	}

	game->brick = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);

	game->man.x = 320 - 40;
	game->man.y = 240 - 40;

	// init crystals
	for (int i = 0; i < 100; i++)
	{
		game->crystals[i].x = rand()%640;
		game->crystals[i].y = rand()%480;
	}

	// init ledges
	for (int i = 0; i < 100; i++)
	{
		game->ledges[i].w = 256;
		game->ledges[i].h = 64;
		game->ledges[i].x = i*256;
		game->ledges[i].y = 400;
	}
	game->ledges[99].x = 350;
	game->ledges[99].y = 200;
}

//useful utility function to see if two rectangles are colliding at all
int collide2d(float x1, float y1, float x2, float y2, float wt1, float ht1, float wt2, float ht2)
{
	return(!((x1 > (x2 + wt2)) || (x2 > (x1 + wt1)) || (y1 > (y2 + ht2)) || (y2 > (y1 + ht1))));
}

void collisionDetect(GameState* game)
{
	// Check for collision with any  ledges (bricks clocks)
	for (int i = 0; i < 100; i++)
	{
		float mw = 48, mh = 48;
		float mx = game->man.x, my = game->man.y;
		float bx = game->ledges[i].x, by = game->ledges[i].y, bw = game->ledges[i].w, bh = game->ledges[i].h;

		if (my < by+bh && my+mh > by)
		{
			// tocco sul lato destro
			if (mx < bx+bw && mx+mw > bx+bw)
			{
				// correggo x
				game->man.x = bx+bw;
				mx = bx+bw;
			}
			else if (mx+mw > bx && mx < bx)
			{
				// correggo x
				game->man.x = bx-mw;
				mx = bx-mx;
			}

		}
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
	// Set render color to blue
	SDL_SetRenderDrawColor(renderer, 128, 128, 255, 255);

	// Clear the screen
	SDL_RenderClear(renderer);

	// Set render color to white
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	//draw the ledges
	for (int i = 0; i < 100; i++)
	{
		SDL_Rect ledgeRect = { game->ledges[i].w,game->ledges[i].h,game->ledges[i].x,game->ledges[i].y };
		SDL_RenderCopy(renderer, game->brick, NULL, &ledgeRect);
	}

	// draw rectangle at man position
	SDL_Rect rect = { game->man.x,game->man.y,48,48 };
	SDL_RenderCopyEx(renderer, game->menFrames[0], NULL, &rect, 0, NULL, 0);

	//draw the crystal image
	//for (int i = 0; i < 100; i++)
	//{
	//	SDL_Rect crystalRect = { game->crystals[i].x,game->crystals[i].y,64,64};
	//	SDL_RenderCopy(renderer, game->crystal, NULL, &crystalRect);
	//}

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

		collisionDetect(&gameState);

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