#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>

#define GRAVITY 0.35f

typedef struct
{
	float x, y;
	float dx, dy;
	short life;
	char* name;
	int onLedge;

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

	int time;

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
	game->man.dx = 0;
	game->man.dy = 0;
	game->man.onLedge = 0;
	game->man.animFrame = 0;
	game->man.facingRight = 0;
	game->man.slowingDown = 0;

	game->time = 0;

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

	game->ledges[98].x = 350;
	game->ledges[98].y = 350;
}

//useful utility function to see if two rectangles are colliding at all
int collide2d(float x1, float y1, float x2, float y2, float wt1, float ht1, float wt2, float ht2)
{
	return(!((x1 > (x2 + wt2)) || (x2 > (x1 + wt1)) || (y1 > (y2 + ht2)) || (y2 > (y1 + ht1))));
}

void process(GameState* game)
{
	//add time
	game->time++;

	//man movement
	Man* man = &game->man;
	man->x += man->dx;
	man->y += man->dy;

	if (man->dx != 0 && man->onLedge && !man->slowingDown)
	{
		if (game->time % 10 == 0)
		{
			if (man->animFrame == 0)
			{
				man->animFrame = 1;
			}
			else
			{
				man->animFrame = 0;
			}
		}
	}

	man->dy += GRAVITY;
}

void collisionDetect(GameState* game)
{
	// Check for collision with any  ledges (bricks clocks)
	for (int i = 0; i < 100; i++)
	{
		float mw = 48, mh = 48;
		float mx = game->man.x, my = game->man.y;
		float bx = game->ledges[i].x, by = game->ledges[i].y, bw = game->ledges[i].w, bh = game->ledges[i].h;

		if (mx+mw /2 > bx && mx+mw /2 < bx+bw)
		{
			// controllo se sbatte la capoccia
			if (my < by+bh && my > by && game->man.dy < 0)
			{
				//correggo y
				game->man.y = by+bh;
				my = by+bh;

				// fermo il salto
				game->man.dy = 0;
				game->man.onLedge = 1;
			}
		}
		//controllo se intersecano sull'asse x
		if (mx+mw > bx && mx < bx+bw)
		{
			// controllo se sto per terra
			if (my+mh > by && my < by && game->man.dy > 0)
			{
				//correggo y
				game->man.y = by-mh;
				my = by-mh;

				// fermo la caduta
				game->man.dy = 0;
				game->man.onLedge = 1;
			}
		}
		//controllo se intersecano sull'asse y
		if (my+mh > by && my<by+bh)
		{
			// m interseca a sinistra, quindi tocca il muro a sx
			if (mx < bx+bw && mx+mw > bx+bw && game->man.dx < 0)
			{
				// correggo x
				game->man.x = bx+bw;
				mx = bx+bw;

				game->man.dx = 0;
			}
			else if (mx+mw > bx && mx < bx && game->man.dx > 0) // m interseca a destra, quindi tocca il muro a dx
			{
				// correggo x
				game->man.x = bx-mw;
				mx = bx-mx;

				game->man.dx = 0;
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
			case SDLK_UP:
				if (game->man.onLedge)
				{
					game->man.dy = -8;
					game->man.onLedge = 0;
				}
			}
		}
		break;
		case SDL_QUIT:
			done = 1;
			break;
		}
	}


	//More jumping
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_UP])
	{
		game->man.dy -= 0.2f;
	}

	//walking
	if (state[SDL_SCANCODE_LEFT])
	{
		game->man.dx -= 0.5;
		if (game->man.dx < -6)
		{
			game->man.dx = -6;
		}
		game->man.facingRight = 0;
		game->man.slowingDown = 0;
	}
	else if (state[SDL_SCANCODE_RIGHT])
	{
		game->man.dx += 0.5;
		if (game->man.dx > 6)
		{
			game->man.dx = 6;
		}
		game->man.facingRight = 1;
		game->man.slowingDown = 0;
	}
	else {
		game->man.animFrame = 0;
		game->man.dx *= 0.8f;
		game->man.slowingDown = 1;
		if (fabsf(game->man.dx) < 0.1f)
		{
			game->man.dx = 0;
		}
	}
	//if (state[SDL_SCANCODE_UP])
	//{
	//	game->man.y -= 10;
	//}
	//if (state[SDL_SCANCODE_DOWN])
	//{
	//	game->man.y += 10;
	//}
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
		SDL_Rect ledgeRect = { game->ledges[i].x,game->ledges[i].y,game->ledges[i].w,game->ledges[i].h };
		SDL_RenderCopy(renderer, game->brick, NULL, &ledgeRect);
	}

	// draw rectangle at man position
	SDL_Rect rect = { game->man.x,game->man.y,48,48 };
	SDL_RenderCopyEx(renderer, game->menFrames[game->man.animFrame], NULL, &rect, 0, NULL, game->man.facingRight);

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

		process(&gameState);
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