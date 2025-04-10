#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>

typedef struct { int x, y; } Vector2;

#define UNIT 24
#define ROWS 9
#define COLS 18

const int DELAY = 4;
Vector2 SCREEN_SIZE;

int counter = 0;
int fast_counter = 1;
int default_counter = 20;
int max_counter;

int i, j, k, l, t, most_down_block, MIDX, COMPLETED_LINES = 0, limit = 3;
bool b;
char s[9];
TTF_Font* font;
SDL_Rect r;
SDL_Color White = { 255, 255, 255, 255 };

// Ground Atributes
bool ground [ROWS][COLS];

// Player Atributes
Vector2 player_block[4];
const int player_block_units = 4;
Vector2 player_pos;

Vector2 L[4] = {
	{ 0, -1 },
	{ 0, 0 },
	{ 0, 1 },
	{ 1, 1 }
};

Vector2 I[4] = {
	{ 0, -1 },
	{ 0, 0 },
	{ 0, 1 },
	{ 0, 2 }
};

Vector2 Q[4] = {
	{ 0, 0 },
	{ 1, 0 },
	{ 0, 1 },
	{ 1, 1 }
};

Vector2 T[4] = {
	{ -1, 0 },
	{ 0, -1 },
	{ 0, 0 },
	{ 1, 0 }
};

Vector2 S[4] = {
	{ -1, 1 },
	{ 0, 1 },
	{ 0, 0 },
	{ 1, 0 }
};

int forms_size = 5;
Vector2* forms[5] = { L, I, Q, T, S };

void player_reset() {
	for(i = 0; i < player_block_units; i++ ) {
		ground[ ( player_pos.x + player_block[i].x ) ][ ( player_pos.y + player_block[i].y ) ] = true;
	}

	player_pos.x = MIDX;
	player_pos.y = 1;

	int rand_num = rand() % forms_size;

	for(i = 0; i < player_block_units; i++)
		player_block[i] = forms[rand_num][i];
}

void awake() {
	srand(time(NULL));

	SCREEN_SIZE.x = UNIT*ROWS;
	SCREEN_SIZE.y = UNIT*COLS;
	MIDX = ( ROWS/2 )-1;

	max_counter = default_counter;
}

void close() {
	TTF_CloseFont(font);
	TTF_Quit();
}

void start() {
	font = TTF_OpenFont("dogicapixel.ttf", 64);

	for(i = 0; i < player_block_units; i++)
		player_block[i] = L[i];

	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLS; j++) {
			ground[i][j] = false;
		}
	}

	player_pos.x = MIDX;
}

void update() {
	for (i = 0; i < COLS; i++) {
		b = true;

		for (j = 0; j < ROWS; j++) {
			if ( ground[j][i] == false ) {
				b = false;
				break;
			}
		}

		if ( b ) {
			COMPLETED_LINES++;

			for (j = 0; j < ROWS; j++) {
				ground[j][i] = false;
			}

			for (k = i; k > 0; k--) {
				for (l = ROWS; l >= 0; l--) {
					if (ground[l][k] == false)
						continue;

					ground[l][k] = false;
					ground[l][k+1] = true;
				}
			}
		}
	}

	// Collision Ground
	most_down_block = player_block[0].y;
	for(i = 1; i < 4; i++){
		if ( player_block[i].y > most_down_block ) {
			most_down_block = player_block[i].y;
		}
	}

	most_down_block = most_down_block + player_pos.y;
	if ( most_down_block >= COLS-1 ){
		player_reset();
		return;
	}

	// Collision With Other Blocks
	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLS; j++) {
			if ( ground[i][j] == false )
				continue;

			b = false;

			for(k = 0; k < player_block_units; k++){
				if ( player_pos.x + player_block[k].x == i && player_pos.y + player_block[k].y + 1 == j ) {
					b = true;
					break;
				}
			}

			if ( b ) {
				player_reset();
				return;
			}
		}
	}

	// Move Player
	player_pos.y++;
}

void draw(SDL_Renderer *render) {
	SDL_RenderClear( render );

	for (i = 0; i < player_block_units; i++) {
		r.x = (player_pos.x + player_block[i].x) * UNIT;
		r.y = (player_pos.y + player_block[i].y) * UNIT;
		r.w = UNIT;
		r.h = UNIT;

		SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
		SDL_RenderDrawRect(render, &r);
	}

	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLS; j++) {
			if ( ground[i][j] == false )
				continue;

			r.x = i * UNIT;
			r.y = j * UNIT;
			r.w = UNIT;
			r.h = UNIT;

			SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
			SDL_RenderFillRect(render, &r);

			SDL_SetRenderDrawColor(render, 230, 230, 230, 255);
			SDL_RenderDrawRect(render, &r);
		}
	}

	SDL_SetRenderDrawColor(render, 255, 165, 0, 255);
	SDL_RenderDrawLine(render, 0, UNIT * limit, SCREEN_SIZE.x, UNIT * limit);

	sprintf(s, "%d", COMPLETED_LINES);
	SDL_Surface *text_surface = TTF_RenderText_Solid(font, s, White);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(render, text_surface);

	r.x = 5;
	r.y = 5;
	r.w = 12;
	r.h = 12;

	SDL_RenderCopy(render, texture, NULL, &r);
	SDL_RenderPresent(render);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(text_surface);

	SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	SDL_RenderPresent(render);
}

void input( SDL_Keycode keyCode ) {
	switch (keyCode){

	// Go right
	case SDLK_LEFT:
		player_pos.x--;
		break;

	// Go left
	case SDLK_RIGHT:
		player_pos.x++;
		break;

	// Go down
	case SDLK_DOWN:
		max_counter = fast_counter;
		break;

	// Rotate
	case SDLK_UP:
		for (i = 0; i < 4; i++) {
			Vector2 new_pos = {
				player_block[i].y * -1,
				player_block[i].x * ( player_block[i].x == 0 ? -1 : 1 )
			};

			player_block[i] = new_pos;
		}
		break;
	}
}

void uninput( SDL_Keycode keyCode ) {
	switch(keyCode) {
		// Go down
		case SDLK_DOWN:
			max_counter = default_counter;
			break;
	}
}

int main() {
	awake();

	// Initialize SDL2
	SDL_Init( SDL_INIT_EVERYTHING );

	// Initialize SDL2_TTF
	TTF_Init();

	// Create Window
	SDL_Window *window = SDL_CreateWindow("TETRIS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_SIZE.x, SCREEN_SIZE.y, SDL_WINDOW_ALLOW_HIGHDPI );
	if ( window == ((void*)0) ) {
		printf( "Error creating window: %s", SDL_GetError() );
		return 1;
	}

	// Create a Render
	SDL_Renderer *render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	if ( render == ((void*)0) ) {
		printf( "Error creating renderer: %s", SDL_GetError() );
		return 1;
	}

	// Create SDL event
	SDL_Event window_event;

	start();

	while ( true ) {
		// Check if QUIT EVENT is triggered
		if ( SDL_PollEvent( &window_event ) ) {
			if ( SDL_QUIT == window_event.type )
				break;
			else if (window_event.type == SDL_KEYDOWN)
				input( window_event.key.keysym.sym );
			else if (window_event.type == SDL_KEYUP)
				uninput( window_event.key.keysym.sym );
		}

		draw(render);

		// call update method
		if ( counter > max_counter ) {
			update();
			counter = 0;
		} else
			counter++;

		SDL_Delay( DELAY );
	}

	// Close window
	SDL_DestroyWindow( window );

	close();

	// CLOSE SDL2
	SDL_Quit();
	return EXIT_SUCCESS;
}