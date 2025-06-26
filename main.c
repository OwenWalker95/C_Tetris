#include <stdio.h>

#include "ow_util.h"
#include "ow_display.h"
#include "ow_input.h"
#include "ow_time.h"
#include "ow_strings.h"

// Global constants
#define GRID_W 10 // Grid blocks wide, dont intend on changing from classic 10x20 but for good practice...
#define GRID_H 20 // Grid blocks high

#define BLOCK_SIZE 25 // in px
#define GRID_X 820 // top left x value
#define GRID_Y 200 // top left y value

#define GRID_COL 75 // rgb value all same - 20 for a dull grey, for now

#define STACK_S 5 // number of next tetrominos to queue up
#define STACK_SHOW 2 // how many 'next' tetrominoes to show
#define STACK_X GRID_X+((GRID_W+3)*BLOCK_SIZE) // top left x for stack display, the + 3 allows for border round grid&stack
#define STACK_Y GRID_Y // top left y for stack display

#define START_SPEED 500 // fall speed when starting
#define SPEED_INCREMENT 10 // how much to speed up each time
#define SPEED_THRESHOLD 10 // how much score is required to reach next level

// data structures - tetromino, grid, 'next' stack
struct tetromino_def
{
	int shape; // 0 - 6 , determines shape & color
	int rotation; // 0 - 3 , determines orientation (applies more or less depending on shape)
	
	// - co-ordinates for each of the blocks making the tetromino.
	vector2 b[4];
};
typedef struct tetromino_def tetromino;

struct stack_def
{
	tetromino T[STACK_S];
};
typedef struct stack_def stack;

struct grid_cell_def
{
	vector2 coords; // position - possibly not needed though
	int block; // 0/1 is there a block on there 
	vector3 color; // 0 - 6 which color
};	
typedef struct grid_cell_def grid_cell;

struct grid_def
{
	grid_cell cell[GRID_W*GRID_H]; // array of grid cells - index can be derived from the coordinates, or vice versa
};
typedef struct grid_def grid;

enum gamestates
{
    INIT,
    START,
    PLAY,
    GAMEOVER,
    EXIT
};
typedef enum gamestates gc_State;

struct gc
{
    gc_State state;
	int speed;
	int speed_score;
	int score;
};
typedef struct gc game_controler;

void init_grid(working_G)
grid *working_G;
{
	for (int i = 0; i < GRID_W * GRID_H; i++)
	{
		working_G->cell[i].block = 0;
	}
}

vector3 get_colorT(working_T)
tetromino *working_T;
{
	vector3 col;
	switch (working_T->shape)
	{
		case 0:
			col = Vector3(100, 181, 246);
			return col;
			break;
		case 1:
			col = Vector3(253, 216, 53);
			return col;
			break;
		case 2:
			col = Vector3(171, 71, 188);
			return col;
			break;
		case 3:
			col = Vector3(21, 101, 192);
			return col;
			break;
		case 4:
			col = Vector3(255, 111, 0);
			return col;
			break;
		case 5:
			col = Vector3(102, 187, 106);
			return col;
			break;
		case 6:
			col = Vector3(255, 0, 0);
			return col;
			break;
	}			
}

// Transformations

// special transformation for creating L and Z shape tetrominos
void mirror_tetromino(working_T)
tetromino *working_T;
{
	if (working_T->rotation == 0 || working_T->rotation == 2)
	// mirror along vertical axis	
	{
		for (int i = 0; i < 4; i++)
		{
			switch(working_T->b[i].x)
			{
				case 0:
					working_T->b[i].x = 3;
					break;
				case 1:
					working_T->b[i].x = 2;
					break;
				case 2:
					working_T->b[i].x = 1;
					break;
				case 3:
					working_T->b[i].x = 0;
					break;					
			}
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{		
			switch(working_T->b[i].y)
			{
				case 0:
					working_T->b[i].y = 3;
					break;
				case 1:
					working_T->b[i].y = 2;
					break;
				case 2:
					working_T->b[i].y = 1;
					break;
				case 3:
					working_T->b[i].y = 0;
					break;					
			}
		}		
	}
}



// instantiations - subfunctions for each tetromino shape
void inst_I(working_T)
tetromino *working_T;
{
	
	// First position is upright, left of centre. then rotating clockwise around centre point
	if (working_T->rotation == 0) //vertical
	{
		working_T->b[0] = Vector2(1,0);
		working_T->b[1] = Vector2(1,1);
		working_T->b[2] = Vector2(1,2);
		working_T->b[3] = Vector2(1,3);		
		return;
	}
	if (working_T->rotation == 1) // horiz
	{
		working_T->b[0] = Vector2(0,1);
		working_T->b[1] = Vector2(1,1);
		working_T->b[2] = Vector2(2,1);
		working_T->b[3] = Vector2(3,1);				
		return;
	}
	if (working_T->rotation == 2) //vertical
	{
		working_T->b[0] = Vector2(2,0);
		working_T->b[1] = Vector2(2,1);
		working_T->b[2] = Vector2(2,2);
		working_T->b[3] = Vector2(2,3);		
		return;		
	}
	if (working_T->rotation == 3) // horiz
	{
		working_T->b[0] = Vector2(0,2);
		working_T->b[1] = Vector2(1,2);
		working_T->b[2] = Vector2(2,2);
		working_T->b[3] = Vector2(3,2);				
		return;
	}	
}

void inst_O(working_T)
tetromino *working_T;
{
	// rotation doesn't even matter here
	working_T->b[0] = Vector2(1,1);
	working_T->b[1] = Vector2(1,2);
	working_T->b[2] = Vector2(2,1);
	working_T->b[3] = Vector2(2,2);
}

void inst_T(working_T)
tetromino *working_T;
{
	// 'prototype shape' ie; T = rotation 0. rotations follow clockwise. Apply same rules for all non symetrical shapes
	// For starting pos The right hand angle between the 'stem' and top of the T will be on center point of 4x4 notional grid.
	// Then rotate around the middle block of the top of the 'T' (block 1)
	if (working_T->rotation == 0)
	{
		working_T->b[0] = Vector2(0, 1);
		working_T->b[1] = Vector2(1, 1);
		working_T->b[2] = Vector2(2, 1);
		working_T->b[3] = Vector2(1, 2);
		return;
	}
	
	if (working_T->rotation == 1)
	{
		working_T->b[0] = Vector2(1, 0);
		working_T->b[1] = Vector2(1, 1);
		working_T->b[2] = Vector2(1, 2);
		working_T->b[3] = Vector2(0, 1);
		return;		
	}
	
	if (working_T->rotation == 2)
	{
		working_T->b[0] = Vector2(2, 1);
		working_T->b[1] = Vector2(1, 1);
		working_T->b[2] = Vector2(0, 1);
		working_T->b[3] = Vector2(1, 0);
		return;		
	}

	working_T->b[0] = Vector2(1, 2);
	working_T->b[1] = Vector2(1, 1);
	working_T->b[2] = Vector2(1, 0);
	working_T->b[3] = Vector2(2, 1);		
}

void inst_J(working_T)
tetromino *working_T;
{
	// J will sit on bottom in first position + rotate around block 1
	if (working_T->rotation == 0)
	{
		working_T->b[0] = Vector2(2, 1);
		working_T->b[1] = Vector2(2, 2);
		working_T->b[2] = Vector2(2, 3);
		working_T->b[3] = Vector2(1, 3);
		return;
	}	
	
	if (working_T->rotation == 1)
	{
		working_T->b[0] = Vector2(3, 2);
		working_T->b[1] = Vector2(2, 2);
		working_T->b[2] = Vector2(1, 2);
		working_T->b[3] = Vector2(1, 1);
		return;		
	}
	
	if (working_T->rotation == 2)
	{
		working_T->b[0] = Vector2(2, 3);
		working_T->b[1] = Vector2(2, 2);
		working_T->b[2] = Vector2(2, 1);
		working_T->b[3] = Vector2(3, 1);
		return;		
	}

	working_T->b[0] = Vector2(1, 2);
	working_T->b[1] = Vector2(2, 2);
	working_T->b[2] = Vector2(3, 2);
	working_T->b[3] = Vector2(3, 3);	
	
}

void inst_L(working_T)
tetromino *working_T;
{
	// Basically mirror image of J
	if (working_T->rotation == 0)
	{
		working_T->b[0] = Vector2(1, 1);
		working_T->b[1] = Vector2(1, 2);
		working_T->b[2] = Vector2(1, 3);
		working_T->b[3] = Vector2(2, 3);
		return;
	}	
	
	if (working_T->rotation == 1)
	{
		working_T->b[0] = Vector2(2, 2);
		working_T->b[1] = Vector2(1, 2);
		working_T->b[2] = Vector2(0, 2);
		working_T->b[3] = Vector2(0, 3);
		return;		
	}
	
	if (working_T->rotation == 2)
	{
		working_T->b[0] = Vector2(1, 3);
		working_T->b[1] = Vector2(1, 2);
		working_T->b[2] = Vector2(1, 1);
		working_T->b[3] = Vector2(0, 1);
		return;		
	}

	working_T->b[0] = Vector2(0, 2);
	working_T->b[1] = Vector2(1, 2);
	working_T->b[2] = Vector2(2, 2);
	working_T->b[3] = Vector2(2, 1);	
	
}

void inst_S(working_T)
tetromino *working_T;
{
	// rotate round block 1
	if (working_T->rotation == 0)
	{
		working_T->b[0] = Vector2(0, 2);
		working_T->b[1] = Vector2(1, 2);
		working_T->b[2] = Vector2(1, 1);
		working_T->b[3] = Vector2(2, 1);
		return;
	}	
	
	if (working_T->rotation == 1)
	{
		working_T->b[0] = Vector2(1, 1);
		working_T->b[1] = Vector2(1, 2);
		working_T->b[2] = Vector2(2, 2);
		working_T->b[3] = Vector2(2, 3);
		return;		
	}	
	
	if (working_T->rotation == 2)
	{
		working_T->b[0] = Vector2(2, 2);
		working_T->b[1] = Vector2(1, 2);
		working_T->b[2] = Vector2(1, 3);
		working_T->b[3] = Vector2(0, 3);
		return;
	}		
	
	if (working_T->rotation == 3)
	{
		working_T->b[0] = Vector2(1, 3);
		working_T->b[1] = Vector2(1, 2);
		working_T->b[2] = Vector2(0, 2);
		working_T->b[3] = Vector2(0, 1);
		return;
	}			
}

void inst_Z(working_T)
tetromino *working_T;
{
	// Mirror of S
	if (working_T->rotation == 0)
	{
		working_T->b[0] = Vector2(1, 1);
		working_T->b[1] = Vector2(2, 1);
		working_T->b[2] = Vector2(2, 2);
		working_T->b[3] = Vector2(3, 2);
		return;
	}	
	
	if (working_T->rotation == 1)
	{
		working_T->b[0] = Vector2(2, 0);
		working_T->b[1] = Vector2(2, 1);
		working_T->b[2] = Vector2(1, 1);
		working_T->b[3] = Vector2(1, 2);
		return;		
	}	
	
	if (working_T->rotation == 2)
	{
		working_T->b[0] = Vector2(3, 1);
		working_T->b[1] = Vector2(2, 1);
		working_T->b[2] = Vector2(2, 0);
		working_T->b[3] = Vector2(1, 0);
		return;
	}		
	
	if (working_T->rotation == 3)
	{
		working_T->b[0] = Vector2(2, 2);
		working_T->b[1] = Vector2(2, 1);
		working_T->b[2] = Vector2(3, 1);
		working_T->b[3] = Vector2(3, 0);
		return;
	}			
}

// return tetromino struct of type, with rotation, and coordinates within a 4x4 grid with top left of x,y
tetromino instantiate_tetromino(x, y, type, rotation)
int x, y, type, rotation;
{
	tetromino working_T;
	working_T.shape = type;
	working_T.rotation = rotation;
	switch (type)
	{
		case 0:
			inst_I(&working_T);
			break;
		case 1:
			inst_O(&working_T);
			break;
		case 2:
			inst_T(&working_T);
			break;
		case 3:
			inst_J(&working_T);
			break;
		case 4:
			inst_L(&working_T);
			//mirror_tetromino(&working_T);
			break;
		case 5:
			inst_S(&working_T);
			break;
		case 6:
			inst_Z(&working_T);
			//mirror_tetromino(&working_T);
			break;
	}
	
	// adjust for the given coords
	for (int i = 0; i < 4; i++)
	{
		working_T.b[i] = Vector2_math(working_T.b[i], Vector2(x,y), '+');
	}
	return working_T;
}


int get_gridIndex(coordinates)
vector2 coordinates;
{
	return (coordinates.y*(GRID_W))+coordinates.x;
}


// Does job for now, eventually add a nice background ting and alternate grid colors slightly
void draw_grid(working_G)
grid *working_G;
{
	// draw a lil border for it
	vector3 sq_col = Vector3(0,0,200); // start out with blue color for border, eventually replace with cool png img?
	
	draw_rectangle(Vector2(GRID_X - BLOCK_SIZE, GRID_Y - BLOCK_SIZE), 
	               Vector2(GRID_X + (BLOCK_SIZE * (GRID_W+1)), GRID_Y + (BLOCK_SIZE * (GRID_H+1))),
				   sq_col, 1, 1);

		
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 20; y++)
		{
			// set grid cell color
			if (working_G->cell[get_gridIndex(Vector2(x,y))].block == 1)
			{
				sq_col = working_G->cell[get_gridIndex(Vector2(x, y))].color;
			}
			else
			{
				sq_col = Vector3(GRID_COL,GRID_COL,GRID_COL); // switch to grey for grid
			}
			// draw square at grid_x + blocksize*x, same concept for y
			draw_rectangle(Vector2(GRID_X + (BLOCK_SIZE * x), GRID_Y + (BLOCK_SIZE * y)), 
			               Vector2(GRID_X + (BLOCK_SIZE * (x+1)), GRID_Y + (BLOCK_SIZE * (y+1))), 
						   sq_col, 1, 1);	
  
            // draw a little border
            sq_col.x = sq_col.x - 10;
            if (sq_col.x < 0)
            {
                sq_col.x = 0;
            }
            sq_col.y = sq_col.y - 10;
            if (sq_col.y < 0)
            {
                sq_col.y = 0;
            }
            sq_col.z = sq_col.z - 10;
            if (sq_col.z < 0)
            {
                sq_col.z = 0;
            }            
			draw_rectangle(Vector2(GRID_X + (BLOCK_SIZE * x), GRID_Y + (BLOCK_SIZE * y)), 
			               Vector2(GRID_X + (BLOCK_SIZE * (x+1)), GRID_Y + (BLOCK_SIZE * (y+1))), 
						   sq_col, 0, 1);	            
		}
	}
	present_renderBuffer();
}

void init_stack(next_stack)
stack *next_stack;
{
	// temporary test code, limiting possible tetrominos to Is and Os only, real code is below
	for (int i = 0; i < STACK_S; i++)	
	{
		/*
		working_T.shape = get_randomRange(0, 6);
		working_T.rotation = get_randomRange(0, 3);
		*/
		next_stack->T[i] =  instantiate_tetromino(0, 0, get_randomRange(0, 6), get_randomRange(0, 3));
	}	
	
	
}

// for now i'll show the next 2, with a view to adding a 'swap' bonus in future
// looks abit shit theyre not centred but leave for now
void draw_next(next_stack)
stack *next_stack;
{
	//draw stack area
	vector3 sq_col = Vector3(0,0,200); // give it a blue border also
	draw_rectangle(Vector2(STACK_X - BLOCK_SIZE, STACK_Y - BLOCK_SIZE), 
	               Vector2(STACK_X + (BLOCK_SIZE * (5)), STACK_Y + (BLOCK_SIZE * ((4*STACK_SHOW)+1))),
				   sq_col, 1, 1);	
	
	sq_col = Vector3(GRID_COL,GRID_COL,GRID_COL); // switch to grey for grid
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4*STACK_SHOW; y++)
		{
			draw_rectangle(Vector2(STACK_X + (BLOCK_SIZE * x), STACK_Y + (BLOCK_SIZE * y)), 
			               Vector2(STACK_X + (BLOCK_SIZE * (x+1)), STACK_Y + (BLOCK_SIZE * (y+1))), 
						   sq_col, 1, 1);				
		}
	}
	
	for (int i = 0; i < STACK_SHOW; i++)
	{

		
		// instantiate tetromino, coordinates relative to local 4x4 grid
//		tetromino curr_tetr = instantiate_tetromino(0, 0, stack[i].x, stack[i].y);
		tetromino curr_tetr = next_stack->T[i];
		
		// establish color to use - for now all yellow but will add variation
		sq_col = get_colorT(&curr_tetr);			
		
		// draw the blocks of current tetromino in stack
		for (int b = 0; b < 4; b++)
		{
			draw_rectangle(Vector2(STACK_X + (BLOCK_SIZE * curr_tetr.b[b].x), (STACK_Y) + (BLOCK_SIZE * (curr_tetr.b[b].y+i*4))), 
			               Vector2(STACK_X + (BLOCK_SIZE * (curr_tetr.b[b].x+1)), (STACK_Y) + (BLOCK_SIZE * ((curr_tetr.b[b].y+1)+i*4))), 
						   sq_col, 1, 1);							
		}
	}	
	present_renderBuffer();
}

// returns next tetromino from stack and replenishes stack
tetromino load_Next(next_stack)
stack *next_stack;
{
	tetromino saveNext = next_stack->T[0];
	
	for (int i = 0; i < STACK_S-1; i++)
	{
		next_stack->T[i] = next_stack->T[i+1];
	}
	next_stack->T[STACK_S-1] = instantiate_tetromino(0, 0, get_randomRange(0, 6), get_randomRange(0, 3));
	
	return saveNext;
}

// place the in play tetromino on random starting  position on grid
void init_gridPos(live_Tetromino)
tetromino *live_Tetromino;
{
	// randomize x position - position the furthest left block
	// need to know the furthest left & furthest right blocks
	int f_left, f_right;
	for (int i = 0; i < 4; i++)
	{
		if (i == 0)
		{
			f_left = i;
			f_right = i;
		}
		else
		{
			if (live_Tetromino->b[i].x < live_Tetromino->b[f_left].x)
			{
				f_left = i;
			}

			if (live_Tetromino->b[i].x > live_Tetromino->b[f_right].x)
			{
				f_right = i;
			}			
		}
	}
	// need to know the width, difference in x betwen furthest left & right
	int t_width = live_Tetromino->b[f_right].x - live_Tetromino->b[f_left].x;
	// random range will be 0 - GRID_W-1 minus the width
	int new_x = get_randomRange(0, GRID_W - (t_width+1));
	// calculate difference between furthest left's x and the new x, apply that transformation to all
	int x_diff = new_x - live_Tetromino->b[f_left].x;
	
	for (int i = 0; i< 4; i++)
	{
		live_Tetromino->b[i].x = live_Tetromino->b[i].x + x_diff;
	}
	
	// lift so that lowest block(s) are at grid pos -1, ie; 1 block above grid
	// find lowest block, find difference betweeen that and -1, apply transformation to all
	int f_low;
	for (int i = 0; i < 4; i++)
	{
		if (i == 0)
		{
			f_low = i;
		}
		else
		{
			if (live_Tetromino->b[i].y > live_Tetromino->b[f_low].y)
			{
				f_low = i;
			}
		}
	}
	
	int y_diff = -1 - live_Tetromino->b[f_low].y;
	for (int i = 0; i< 4; i++)
	{
		live_Tetromino->b[i].y = live_Tetromino->b[i].y + y_diff;
	}	
	
}

// clear current tetromino, for calling before any moves to remove from grid before redrawing in new pos
void clear_Tetromino(live_Tetromino)
tetromino *live_Tetromino;
{
	vector3 sq_col;
	for (int b = 0; b < 4; b++)
	{
        sq_col = Vector3(GRID_COL,GRID_COL,GRID_COL);
		// x will always be in bounds but don't do anything if not dropped into grid yet
		if (live_Tetromino->b[b].y < 0)
		{
			continue;
		}
		draw_rectangle(Vector2(GRID_X + (BLOCK_SIZE * live_Tetromino->b[b].x), (GRID_Y) + (BLOCK_SIZE * live_Tetromino->b[b].y)), 
		               Vector2(GRID_X + (BLOCK_SIZE * (live_Tetromino->b[b].x+1)), (GRID_Y) + (BLOCK_SIZE * (live_Tetromino->b[b].y+1))), 
					   sq_col, 1, 1);	

        sq_col.x = sq_col.x - 10;
        if (sq_col.x < 0)
        {
            sq_col.x = 0;
        }
        sq_col.y = sq_col.y - 10;
        if (sq_col.y < 0)
        {
            sq_col.y = 0;
        }
        sq_col.z = sq_col.z - 10;
        if (sq_col.z < 0)
        {
            sq_col.z = 0;
        }   
		draw_rectangle(Vector2(GRID_X + (BLOCK_SIZE * live_Tetromino->b[b].x), (GRID_Y) + (BLOCK_SIZE * live_Tetromino->b[b].y)), 
		               Vector2(GRID_X + (BLOCK_SIZE * (live_Tetromino->b[b].x+1)), (GRID_Y) + (BLOCK_SIZE * (live_Tetromino->b[b].y+1))), 
					   sq_col, 0, 1);	        
	}	
//	present_renderBuffer();	
}

// draw tetromino on grid - will have counterpart to clear tetromino, saving having to draw entire grid each time
void draw_Tetromino(live_Tetromino)
tetromino *live_Tetromino;
{
	vector3 sq_col;
	for (int b = 0; b < 4; b++)
	{
        sq_col = get_colorT(live_Tetromino);
		// x will always be in bounds but don't display block if not dropped into grid yet
		if (live_Tetromino->b[b].y < 0)
		{
			continue;
		}
		draw_rectangle(Vector2(GRID_X + (BLOCK_SIZE * live_Tetromino->b[b].x), (GRID_Y) + (BLOCK_SIZE * live_Tetromino->b[b].y)), 
		               Vector2(GRID_X + (BLOCK_SIZE * (live_Tetromino->b[b].x+1)), (GRID_Y) + (BLOCK_SIZE * (live_Tetromino->b[b].y+1))), 
					   sq_col, 1, 1);	

        sq_col.x = sq_col.x - 10;
        if (sq_col.x < 0)
        {
            sq_col.x = 0;
        }
        sq_col.y = sq_col.y - 10;
        if (sq_col.y < 0)
        {
            sq_col.y = 0;
        }
        sq_col.z = sq_col.z - 10;
        if (sq_col.z < 0)
        {
            sq_col.z = 0;
        }            
        // draw a little border
		draw_rectangle(Vector2(GRID_X + (BLOCK_SIZE * live_Tetromino->b[b].x), (GRID_Y) + (BLOCK_SIZE * live_Tetromino->b[b].y)), 
		               Vector2(GRID_X + (BLOCK_SIZE * (live_Tetromino->b[b].x+1)), (GRID_Y) + (BLOCK_SIZE * (live_Tetromino->b[b].y+1))), 
					   sq_col, 0, 1);	                       
	}	
//	present_renderBuffer();
}


//shift tetromino left or right
void strafe(working_T, working_G, dir)
tetromino *working_T;
grid *working_G;
char dir;
{
	// make copy of tetromino to apply transformation to
	tetromino temp_T = *working_T;
	
	int transf;
	
	if (dir == 'l')
	{
		transf = -1;
	}
	else
	{
		transf = 1;
	}
	
	for (int i = 0; i < 4; i++)
	{
		temp_T.b[i].x = temp_T.b[i].x + transf;
		
		// check if this'll be out of bounds, if so return
		if (temp_T.b[i].x < 0 || temp_T.b[i].x >= GRID_W)
		{
			return;
		}
		// check if this'll hit another block, if so return
		if (working_G->cell[get_gridIndex(Vector2(temp_T.b[i].x,temp_T.b[i].y))].block == 1)
		{
			return;
		}
	}
	
	// all clear so update the man
	clear_Tetromino(working_T);
	*working_T = temp_T;
	draw_Tetromino(working_T);
	present_renderBuffer();
}

void drop_Tetromino(working_T)
tetromino *working_T;
{
	clear_Tetromino(working_T);
	for (int i = 0; i < 4; i++)
	{
		working_T->b[i].y++;
	}
	draw_Tetromino(working_T);
	
	present_renderBuffer();
}


int check_landed(working_T, working_G)
tetromino *working_T;
grid *working_G;
{
	for (int i = 0; i < 4; i++)
	{
		// do not check if block isn't on the actual grid yet
		if (working_T->b[i].y < 0)
		{
			continue;
		}
		
		//first check if hit bottom row		
		if (working_T->b[i].y == GRID_H-1)
		{
			return 1;
		}
		// now check if collided with a previously landed block, check block below on grid to see if full
		if (working_G->cell[get_gridIndex(Vector2(working_T->b[i].x,working_T->b[i].y+1))].block == 1)
		{
			return 1;
		}
	}
	return 0;
}

int lock_toGrid(working_T, working_G)
tetromino *working_T;
grid *working_G;
{	
    int fin = 0;
	for (int i = 0; i < 4; i++)
	{
		// flag if reached top row, ie; game over
		if (working_T->b[i].y == 0)
		{
			fin = 1;
		}
		// get the grid index for this block
		// populate the grid struct with the color
		working_G->cell[get_gridIndex(working_T->b[i])].block = 1;
		working_G->cell[get_gridIndex(working_T->b[i])].color = get_colorT(working_T);
	}		
	return fin;
}

// Reposition a tetromino that's been rotated into an invalid position
// find the closest position to the original position which is valid
// return 0 for success. return 1 if couldnt repositions
int reposition(original_T, new_T, working_G)
tetromino *original_T, *new_T;
grid *working_G;
{
//	printf("start\n");
	// find original position's center point 
	vector2 lows = original_T->b[0];
	vector2 his  = original_T->b[0];
//	printf("hi/lo vectors initialised\n");
	for (int i = 0; i < 4; i++)
	{
//		printf("(%d,%d)\n", original_T->b[i].x, original_T->b[i].y);
		if (original_T->b[i].x < lows.x)
		{
			lows.x = original_T->b[i].x;
		}
		else if (original_T->b[i].x > his.x)
		{
			his.x = original_T->b[i].x;
		}
		
		if (original_T->b[i].y < lows.y)
		{
			lows.y = original_T->b[i].y;
		}
		else if (original_T->b[i].y > his.y)
		{
			his.y = original_T->b[i].y;
		}		
	}
	
	double c_x = (lows.x + his.x) / 2.0;
	double c_y = (lows.y + his.y) / 2.0;
//	printf("avg = (%f,%f)\n", c_x, c_y);
//	printf("avg orig pos found\n");
	// make a temp T to start repositioning.
	tetromino temp_T;
//	printf("temp_T initialised\n");	
	double new_c_x;
	double new_c_y;	

	double x_diff;
	double y_diff;
	double tot_diff;	
	
	int position_found = 0;
	
	double best_diff;
	int best_dist;
	int best_x;
	int best_y;
//	printf("loop vars declared\n");	
	// do up to 2 passes (for now)
//	printf("start loop\n");	
	for (int i = 0; i < 2; i++)
	{
//		printf("start loop instance: %d\n", i);
		best_diff = 0;
		best_diff = 0;
		best_x = 0;
		best_y = 0;
		// 8 potential positions
		for (int x = -1; x <= 1 ; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				temp_T = *new_T;
//				printf("trying pos: (%d, %d)\n", x, y);
				vector2 transf = Vector2(x * (i+1), y * (i+1));
//				printf("set transformation (%d, %d)\n", transf.x, transf.y);
				int valid = 0;				
				// set new location, keep track of positions for working out avg
				for (int b = 0; b < 4; b++)
				{
					temp_T.b[b] = Vector2_math(temp_T.b[b], transf, '+');
//					printf("block %d -> (%d, %d)\n", b, temp_T.b[b].x, temp_T.b[b].y);
					if (b == 0)
					{
						lows = temp_T.b[0];
						his  = temp_T.b[0];
					}
					
					if (temp_T.b[b].x < lows.x)
					{
						lows.x = temp_T.b[b].x;
					}
					else if (temp_T.b[b].x > his.x)
					{
						his.x = temp_T.b[b].x;
					}
					
					if (temp_T.b[b].y < lows.y)
					{
						lows.y = temp_T.b[b].y;
					}
					else if (temp_T.b[b].y > his.y)
					{
						his.y = temp_T.b[b].y;
					}	

					// check if this position is valid

				
					//check if out of bounds
					if (temp_T.b[b].x < 0 || temp_T.b[b].x >= GRID_W || temp_T.b[b].y >= GRID_H)
					{
						valid = 1;
					}
		
					//check if hit blocks
					if (working_G->cell[get_gridIndex(Vector2(temp_T.b[b].x,temp_T.b[b].y))].block == 1)
					{
						valid = 1;
					}
//					printf("validity checked\n");					
					
				}
//				printf("applied transformation\n");
				

				// if valid, find out the avg pos, and how close that is to the original pos


				if (valid == 0)
				{
					new_c_x = (lows.x + his.x) / 2.0;					
					new_c_y = (lows.y + his.y) / 2.0;

					x_diff = new_c_x - c_x;
					y_diff = new_c_y - c_y;
					
					// normalise differences
					if (x_diff < 0)
					{
						x_diff = x_diff * -1;
					}
					if (y_diff < 0)
					{
						y_diff = y_diff * -1;
					}			
						
					tot_diff = x_diff + y_diff;	
					
					if (position_found == 0 || tot_diff < best_diff)
					{
						best_diff = tot_diff;
						
						best_dist = i+1;
						best_x = x;
						best_y = y;
						
						position_found = 1;
					}
//					printf("* best effort recorded\n");
					
				}

			}
		}
		// pass done.
		// if position has been found, update the position and return
//		printf("pass finished\n");
		if (position_found == 1 && best_diff < 3.0) // keep an eye on this, might be too restricted, if unable to rotate pieces when should be, this probs why
		{
//			printf("repo dist = %f\n", best_diff);
//			printf("update and return\n");
			for (int b = 0; b < 4; b++)
			{
				new_T->b[b].x = new_T->b[b].x + (best_x * best_dist);
				new_T->b[b].y = new_T->b[b].y + (best_y * best_dist);
			}
			return 0;
		}
	}
	//printf("no reposition found\n");

	return 1;
}

void rotate_T(working_T, working_G, dir)
tetromino *working_T;
grid *working_G;
char dir;
{
	if (working_T->shape == 1)
	{
		// pointless if square as no rotation
		return;
	}
	//printf("%d: start\n",get_timer());
	// find the new rotation
	int new_rot;
	switch (dir)
	{
		// l = anti-clockwise
		case 'l':
			new_rot = working_T->rotation - 1;
			if (new_rot == -1)
			{
				new_rot = 3;
			}			
			break;
		case 'r':
			new_rot = working_T->rotation + 1;
			if (new_rot == 4)
			{
				new_rot = 0;
			}			
			break;
		default:
			return;
			break;
	}
	// make 2 temp tetrominos, 1 with the current rotation and 1 with the new.
	tetromino tempT_curr = instantiate_tetromino(0, 0, working_T->shape, working_T->rotation);
	tetromino tempT_new = instantiate_tetromino(0, 0, working_T->shape, new_rot);
	
	// make copy of tetromino to apply transformation to
	tetromino trans_T = *working_T;
	// apply the transformation between them to the temporary tetromino
	int valid = 0;
	for (int i = 0; i < 4; i++)
	{
		vector2 diff_V = Vector2_math(tempT_new.b[i], tempT_curr.b[i], '-');
		trans_T.b[i] = Vector2_math(trans_T.b[i], diff_V, '+');
		
		//check if out of bounds
		if (trans_T.b[i].x < 0 || trans_T.b[i].x >= GRID_W || trans_T.b[i].y >= GRID_H)
		{
			valid = 1;
		}
		
		//check if hit blocks
		if (working_G->cell[get_gridIndex(Vector2(trans_T.b[i].x,trans_T.b[i].y))].block == 1)
		{
			valid = 1;
		}
	}	
	
	if (valid == 1)
	{
		if (reposition(working_T, &trans_T, working_G) == 1)
		{
			return;
		}
	}
	// check if feasible
	// thinking check if clipped into sidewalls, if so try to reposition horizantly, if can do so
	// .. without hitting something else then ok - if not abort rotation
	// same for landing on bottom or blocks, shift up with same checks
	
	// apply the transformation to live tetromino and display
	clear_Tetromino(working_T);
	*working_T = trans_T;	
	working_T->rotation = new_rot;
	draw_Tetromino(working_T);	
	present_renderBuffer();
}

void input_loop(working_T, working_G, control, exit_Button)
tetromino *working_T;
grid *working_G;
game_controler *control;
clickable_rect *exit_Button;
{
	ow_input_type input_T;
	ow_keyButton input_K;
    vector2 input_pos;
	
	start_timer();
	
	while(get_timer() < control->speed)
	{
		input_T = get_input();		
		if (input_T == KEYPRESS_DOWN)
		{
			//printf("gotkey\n");
			input_K = get_keyButton();
			switch (input_K)
			{
				case LEFT_ARROW:
					strafe(working_T, working_G, 'l');
					break;
				case RIGHT_ARROW:
					strafe(working_T, working_G, 'r');
					break;
				case DOWN_ARROW:
					return;
					break;
				case UP_ARROW:
					rotate_T(working_T, working_G, 'r');
					break;					
				case Z:
				//	printf("rot L\n");
					rotate_T(working_T, working_G, 'l');
					break;
				case X:
				//	printf("rot R\n");
					rotate_T(working_T, working_G, 'r');
					break;
			}
		}
        else if (input_T == LEFT_CLICK_DOWN)
        {
            input_pos = get_mouseClickPosition();
/*            
            if (input_pos.x >= exit_Button->topleft.x && input_pos.y >= exit_Button->topleft.y)
            {
                if (input_pos.x <= exit_Button->bottomright.x && input_pos.y <= exit_Button->bottomright.y)
                {
                    control->state = EXIT;
                    return;
                }
            }            
*/            
            if (check_clickableRect(input_pos, exit_Button) == 1)
            {
                control->state = EXIT;
                return;                
            }
        }
	}
}

int play_Tetromino(working_T, working_G, control, exit_Button)
tetromino *working_T;
grid *working_G;
game_controler *control;
clickable_rect *exit_Button;
{
	int falling = 1; //? for now.. lol
//	start_timer();
	while (falling == 1)
	{
//		Get user inputs
//		input_loop(working_T, working_G);
		
//      Check before drop in case user has moved it to a 'landed' position		
		if (check_landed(working_T, working_G) == 0)
		{
			drop_Tetromino(working_T);
			if (check_landed(working_T, working_G) == 1)
			{
				falling = 0;
			}	
		//  Having the input after drop seems better as allows brief moment to 'slide' once landed
		//  Get user inputs
			input_loop(working_T, working_G, control, exit_Button);
            if (control->state == EXIT)
            {
                return 1;
            }
		// check landed again though just incase 'lifted off' again by rotating
			if (falling == 0)
			{
				if (check_landed(working_T, working_G) != 1)
				{
					falling = 1;
				}
			}
		}
		else
		{
			falling = 0;
		}
	}

	if (lock_toGrid(working_T, working_G) == 1)
	{
		return 1;
	}
	
	return 0;
}

void drop_pile(low_y, working_G)
int low_y; // lowest row; bottom of the pile to be dropped
grid *working_G;
{
//	printf("dropping pile\n");
	int curr_y = low_y;
	int blank_row = 0;
	int curr_index, new_index;
	while (curr_y > 0 && blank_row == 0)
	{
//		printf("drop y=%d\n", curr_y);
		blank_row = 1;
		for (int x = 0; x < GRID_W; x++)
		{
			curr_index = get_gridIndex(Vector2(x, curr_y));
			// first check if theres a block on it, if so flag this as not a blank row so keep looping
			if (blank_row == 1 && working_G->cell[curr_index].block == 1)
			{
				blank_row = 0;
			}
			// move each cell down 1 and clear current (for the animation)
			new_index = get_gridIndex(Vector2(x, curr_y+1));
			
			working_G->cell[new_index] = working_G->cell[curr_index];
			working_G->cell[curr_index].block = 0;
//			draw_grid(working_G); // draw for every cell move
		}
		if (blank_row == 1)
		{
			break;
		}		
		draw_grid(working_G); // draw for each line move
		// put a delay in here
		curr_y--;
	}
	//draw_grid(working_G); // draw for full drop
}

void increase_speed(control)
game_controler *control;
{
	control->speed = control->speed - SPEED_INCREMENT;
	control->speed_score = control->speed_score + SPEED_THRESHOLD;
}

void display_score(control)
game_controler *control;
{
	char sco[8];
	
	int integer;
	int work_fig = control->score;
	for (int i = 7; i > 0; i--)
	{
		integer = work_fig / num_toPower(10, i);
		sco[8-(i+1)] = integer + '0';
		work_fig = work_fig - (integer * num_toPower(10, i));
	}
	sco[7] = work_fig + '0';
	
	
	//extract the parameters out of these so only need changing in one place for tweaks
	int start_x = GRID_X;
	int start_y = GRID_Y - (BLOCK_SIZE*5);
	
	int digit_size = BLOCK_SIZE*2;
	
	int end_x = start_x + BLOCK_SIZE*8;
	int end_y = start_y + digit_size;
	
	draw_rectangle(Vector2(start_x, start_y), Vector2(end_x, end_y), Vector3(0,0,0), 1, 0);
	draw_text(sco, "data-latin.ttf", digit_size, Vector3(255, 255, 255), Vector2(start_x, start_y));
}

void update_score(lines, control)
int lines;
game_controler *control;
{
	control->score = control->score + (lines * lines);
	display_score(control);
	if (control->score >= control->speed_score)
	{
		increase_speed(control);
	}
}

void remove_lines(start, number, working_G)
int start, number;
grid *working_G;
{
//	printf("remove %d lines from row %d\n", number, start);
	// remove number of lines/rows from (&including) start
	for (int x = 0; x < GRID_W; x++)
	{
		working_G->cell[get_gridIndex(Vector2(x, start))].block = 0;
		
		if (number > 1)
		{
			working_G->cell[get_gridIndex(Vector2(x, start-1))].block = 0;
			if (number > 2)
			{
				working_G->cell[get_gridIndex(Vector2(x, start-2))].block = 0;
				if (number > 3)
				{
					working_G->cell[get_gridIndex(Vector2(x, start-3))].block = 0;
				}
			}
		}
		draw_grid(working_G);
		// should probably add a timed delay here. Drawing the full grid does delay it but better to be in control of that directly.
	}
	
	// drop the blocks pile down
	int start_drop = start-number;
	for (int drops = 0; drops < number; drops++)
	{
		drop_pile(start_drop, working_G);
		start_drop++;
	}
}

void clear_lines(working_G, control)
grid *working_G;
game_controler *control;
{
//	printf("clear\n");
	// check for completed lines, starting at bottom as more likely to be there.
	// if found one, check for 'connected' ones ie; is the next one also completed
	// once found all the connected ones, clear them and drop the pile down.
	// keep checking until 4 away from the original found line, pointless going further as only 4 can be done at once
	int checking = 1;
	int check_y = GRID_H-1;
	int first_row = -1; // -1 will indicate nothing, since 0 is a valid row
	int check_counter = 0; // 0 indicates counter hasn't started. starts counting to 4 once a line is found in order to abort checking 
	int row_cleared; 
	while (checking == 1 && check_y >= 0)
	{
//		printf("checking row %d\n", check_y);
		if (check_counter != 0)
		{
			check_counter++;
			if (check_counter == 5) // count to 5 so we if 4 were cleared were still checking the next one to triggr the removal
			{
//				printf("no more rows can be cleared\n");
				checking = 0;
			}
		}
		
		row_cleared = 1;
		for (int x = 0; x < GRID_W; x++)
		{
//			printf ("%d(%d):%d / ", x, get_gridIndex(Vector2(x,check_y)), working_G->cell[get_gridIndex(Vector2(x,check_y))].block);
			if (working_G->cell[get_gridIndex(Vector2(x,check_y))].block != 1)
			{
				row_cleared = 0;
				break;
			}
		}
		
		if (row_cleared == 1)
		{
//			printf("-cleared row found\n");
//			printf("*row is full*\n");
			if (first_row == -1)
			{
//				printf("--start of cleared rows\n");
				first_row = check_y;
				if (check_counter == 0)
				{
//					printf("--this is the first cleared row\n");
					check_counter = 1;
				}
			}
		}
		else
		{
//			printf("-row is not full\n");
			if (first_row != -1)
			{
//				printf("--streak broken\n");
				remove_lines(first_row, first_row - check_y, working_G);
				update_score(first_row - check_y, control);
				first_row = -1;
			}
		}
		
		check_y--;
	}
	
	
}

void display_start(start_Button)
clickable_rect *start_Button;
{
	int char_size = BLOCK_SIZE * 2;
    int start_x = GRID_X + ((GRID_W*BLOCK_SIZE) - (BLOCK_SIZE*5))/2;
	int start_y = GRID_Y + ((GRID_H*BLOCK_SIZE) - char_size)/2;
	
	draw_rectangle(Vector2(start_x, start_y), Vector2(start_x + (BLOCK_SIZE*5), start_y + char_size), Vector3(255,255,255), 1, 0);
	draw_text("START", "data-latin.ttf", char_size, Vector3(0, 0, 0), Vector2(start_x, start_y));	
    
    // Init the clickable area 
    start_Button->topleft = Vector2(start_x, start_y);
    start_Button->bottomright = Vector2(start_x + (BLOCK_SIZE*5), start_y + char_size);
    start_Button->active = 1;
}

void display_exit(exit_Button)
clickable_rect *exit_Button;
{
	int char_size = BLOCK_SIZE * 2;
    int start_x = BLOCK_SIZE*5;
	int start_y = BLOCK_SIZE*5;
	
	draw_rectangle(Vector2(start_x, start_y), Vector2(start_x + BLOCK_SIZE, start_y + char_size), Vector3(255,0,100), 1, 0);
	draw_text("X", "data-latin.ttf", char_size, Vector3(0, 0, 0), Vector2(start_x, start_y));	
    
    // Init the clickable area 
    exit_Button->topleft = Vector2(start_x, start_y);
    exit_Button->bottomright = Vector2(start_x + BLOCK_SIZE, start_y + char_size);
    exit_Button->active = 1;    
}

// turn all the grid grey when gameover - needs slowing down a bit
void freeze_Grid(Grid)
grid *Grid;
{
    int x, y;
    for (y = 0; y < GRID_H; y++)
    {
        for (x = 0; x < GRID_H; x++)
        {
            if (Grid->cell[get_gridIndex(Vector2(x,y))].block == 1)
            {
                Grid->cell[get_gridIndex(Vector2(x,y))].color = Vector3(175, 175, 175);
            }
        }
        draw_grid(Grid);
    }
}

// 
void display_gameover(restart_Button)
clickable_rect *restart_Button;
{
    // draw 'game over' signage = button for new game
	int char_size = BLOCK_SIZE * 2;
    int start_x = GRID_X + ((GRID_W*BLOCK_SIZE) - (BLOCK_SIZE*9))/2;
	int start_y = GRID_Y + ((GRID_H*BLOCK_SIZE) - char_size)/2;

	draw_text("GAME OVER", "data-latin.ttf", char_size, Vector3(255, 0, 0), Vector2(start_x, start_y));   

    start_x = GRID_X + ((GRID_W*BLOCK_SIZE) - (BLOCK_SIZE*7))/2;
	start_y = (GRID_Y + ((GRID_H*BLOCK_SIZE) - char_size)/2) + BLOCK_SIZE*3;   
	draw_rectangle(Vector2(start_x, start_y), Vector2(start_x + (BLOCK_SIZE*7), start_y + char_size), Vector3(255,255,255), 1, 0);
	draw_text("RESTART", "data-latin.ttf", char_size, Vector3(0, 0, 0), Vector2(start_x, start_y));
    
    restart_Button->topleft = Vector2(start_x, start_y);
    restart_Button->bottomright = Vector2(start_x + (BLOCK_SIZE*7), start_y + char_size);
    restart_Button->active = 1;    
    
}

// TETRIS
int main()
{	
	// INITIALISE
	// Init window
	char wname[30];
	sprintf(wname, "window");
	create_window(0, 0, wname, 1);
	vector2 wsize = get_winSize();
	resize_window(wsize.x, wsize.y);

	// Init input
	clear_input();
	ow_input_type input_type; // are these two used anywhere? feel better to handle input vars localy to avoid confusions
	ow_keyButton input_key;
    ow_input_type getclick;
    vector2 click_pos;    

	// Init random number generator
	seed_rng();		
	
	// Create game objects
	game_controler controller;     // Game state variables
	tetromino live_Tetromino;      // Data for in-play tetromino
	grid live_Grid;	               // Data for game grid
	stack next_stack;              // Queue of next tetrominoes - Note the stack was done to allow showing more than one
                                   //                           - 'next' tetrominos. It is now unlikely that i'll change
                                   //                           - the number shown to anything other than 1 or 2. 
                                   //                           - The idea with showing 2 instead of one was the idea to 
                                   //                           - be able to swap the next two, perhaps available after 
                                   //                           - getting a certain score or number of lines. Probably 
                                   //                           - won't add this feature either but will leave things as
                                   //							- they are in case I ever do come back to this. Number 
                                   //                           - of pieces in stack is 5 and number shown is 2, they're
                                   //                           - controlled by constants at the top. Not tested others. 
    clickable_rect start_Button;   // Clickable area for start button (all initialised + activated when drawn)
    clickable_rect exit_Button;    // Clickable area for exit button
    clickable_rect restart_Button; // Clickable area for new game button
	
    // Set initial gamestate
    controller.state = INIT;
	
	//  GAME LOOP
    while (controller.state != EXIT)
    {
        switch (controller.state)
        {
            case INIT:
                // Reset Game Controller
                controller.state = START;
                controller.speed = START_SPEED;
                controller.speed_score = SPEED_THRESHOLD;
                controller.score = 0;  
                // Initialise Grid
                init_grid(&live_Grid);
                // Display Start screen
                draw_grid(&live_Grid);
                display_score(&controller);
                display_start(&start_Button);
                display_exit(&exit_Button);   
                break;
                
            case START:
                // get click on start

                getclick = NONE;
                click_pos = Vector2(-1, -1);
                
                getclick = get_input();
                if (getclick == LEFT_CLICK_DOWN)
                {
                    click_pos = get_mouseClickPosition();
/*                    
                    if (click_pos.x >= start_Button.topleft.x && click_pos.y >= start_Button.topleft.y)
                    {
                        if (click_pos.x <= start_Button.bottomright.x && click_pos.y <= start_Button.bottomright.y)
                        {
                            clear_input();
                            draw_grid(&live_Grid);
                
                            // initialise stack - say 5 tetrominos for now? defined as a constant at top
                            init_stack(&next_stack);
                            draw_next(&next_stack);                                       
                            controller.state = PLAY;
                       //     clear_input();
                        }
                    }
                    else if (click_pos.x >= exit_Button.topleft.x && click_pos.y >= exit_Button.topleft.y)
                    {
                        if (click_pos.x <= exit_Button.bottomright.x && click_pos.y <= exit_Button.bottomright.y)
                        {
                            controller.state = EXIT;
                        }
                    }
*/
                    if (check_clickableRect(click_pos, &start_Button) == 1)
                    {
                        clear_input();
                        draw_grid(&live_Grid);
                
                        // initialise stack - say 5 tetrominos for now? defined as a constant at top
                        init_stack(&next_stack);
                        draw_next(&next_stack);                                       
                        controller.state = PLAY;
                       //     clear_input();
                    }
                    else if (check_clickableRect(click_pos, &exit_Button) == 1)
                    {
                        controller.state = EXIT;                        
                    }
                }
                break;

            case PLAY:
            // Start one game
                // Sets up next piece
                live_Tetromino = load_Next(&next_stack);
                draw_next(&next_stack);
                init_gridPos(&live_Tetromino);				
                
                // Gameplay loop for 1 piece
                if (play_Tetromino(&live_Tetromino, &live_Grid, &controller, &exit_Button) == 1)
                {
                    if (controller.state != EXIT)
                    {
                        freeze_Grid(&live_Grid);
                        display_gameover(&restart_Button);
                        controller.state = GAMEOVER;
                    }
                }
                
                // Check for and clear completed lines
                clear_lines(&live_Grid, &controller);  
                break;
                
            case GAMEOVER:
                getclick = NONE;
                click_pos = Vector2(-1, -1);
            
                getclick = get_input();
                if (getclick == LEFT_CLICK_DOWN)
                {
                    click_pos = get_mouseClickPosition(); 
/*                    
                    if (click_pos.x >= exit_Button.topleft.x && click_pos.y >= exit_Button.topleft.y)
                    {
                        if (click_pos.x <= exit_Button.bottomright.x && click_pos.y <= exit_Button.bottomright.y)
                        {
                            controller.state = EXIT;
                        }
                    }
*/
                    if (check_clickableRect(click_pos, &exit_Button) == 1)
                    {
                        controller.state = EXIT;
                    }
                    else if (check_clickableRect(click_pos, &restart_Button) == 1)
                    {
                        controller.state = INIT;
                    }
                }
                break;
        }
    }


// done - save as own project and release v2 of ow_units
	
}