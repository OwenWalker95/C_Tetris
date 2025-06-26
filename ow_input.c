// ow_input.c //
//
// general input functions
//
// 25JAN2025 - Started
// 16MAR2025 - V1 Completed
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Current Changes -- V2
//
// AMD001: 28/03/25 - Added diagnostic function to print numeric key identifier  (linked to AMD001 in ow_input.h)
// AMD002: 28/03/25 - Added more return values to the get_keyButton function (linked to AMD002 in ow_input.h)
// AMD003: 15/06/25 - Function to determine if clickable rect was clicked
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// General C includes 
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "ow_input.h"

// Static struct for SDL input functions to use ---------------------------------------------------------
static 	SDL_Event event;
// ------------------------------------------------------------------------------------------------------

// FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////////
// Terminal functions -----------------------------------------------------------------------------------
// Accepts <size> chars from command line into <cli> - returns once user enters \n
// The final char will always be null terminator '\0'
void get_CLI(cli, size)
char *cli;
int size;
{
	fgets(cli, size * sizeof(char),stdin);

	size_t len = strlen(cli);
	
	if (len < size-1)
	{
		// ensure last char is null terminator if less than maximum were entered
		cli[len-1] = '\0';
		len = strlen(cli);
	}
	else
	{
		// only do this if maximum characters were entered
		while (getchar() != '\n');		
	}

	return;
}
// General functions ----------------------------------------------------------------------------------
void clear_input()
{
	SDL_PumpEvents();
	SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
}

ow_input_type get_input()
{
	ow_input_type event_type;
	event_type = NONE;
	while(SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				return QUIT;
				break;
			case SDL_KEYDOWN:
				return KEYPRESS_DOWN;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					return LEFT_CLICK_DOWN;
					break;
				}
				if (event.button.button == SDL_BUTTON_RIGHT)
				{
					return RIGHT_CLICK_DOWN;
					break;
				}
			case SDL_MOUSEMOTION:
				return MOUSE_MOVED;
				break;
		}	

		event_type = NONE;
		return NONE;
	}
}

// Mouse functions ------------------------------------------------------------------------------------
vector2 get_mouseClickPosition()
{
	return Vector2(event.button.x,event.button.y);
}

vector2 get_mouseMovePosition()
{
	return Vector2(event.motion.x,event.motion.y);
}

// Keyboard functions -----------------------------------------------------------------------------------
// AMD001 - START
void print_keyNum()
{
	printf("%d\n",event.key.keysym.sym);	
}
// AMD001 - END

ow_keyButton get_keyButton()
{
// AMD001 - START
    //printf("%d\n",event.key.keysym.sym);
// AMD001 - END
// AMD002 - START
//	if (event.key.keysym.sym == SDLK_ESCAPE)
//	{
//		return ESCAPE;		
//	}

	switch (event.key.keysym.sym)
	{
		case SDLK_ESCAPE:
			return ESCAPE;
			break;
		case 1073741906:
			return UP_ARROW;
			break;
		case 1073741905:
			return DOWN_ARROW;
			break;
		case 1073741904:
			return LEFT_ARROW;
			break;
		case 1073741903:	
			return RIGHT_ARROW;
			break;
		case 97:
			return A;
			break;
		case 98:
			return B;
			break;			
		case 99:
			return C;
			break;		
		case 100:
			return D;
			break;		
		case 101:
			return E;
			break;	
		case 102:
			return F;
			break;		
		case 103:
			return G;
			break;		
		case 104:
			return H;
			break;		
		case 105:
			return I;
			break;		
		case 106:
			return J;
			break;		
		case 107:
			return K;
			break;		
		case 108:
			return L;
			break;		
		case 109:
			return M;
			break;		
		case 110:
			return N;
			break;		
		case 111:
			return O;
			break;		
		case 112:
			return P;
			break;		
		case 113:
			return Q;
			break;		
		case 114:
			return R;
			break;		
		case 115:
			return S;
			break;		
		case 116:
			return T;
			break;		
		case 117:
			return U;
			break;		
		case 118:
			return V;
			break;		
		case 119:
			return W;
			break;		
		case 120:
			return X;
			break;		
		case 121:
			return Y;
			break;		
		case 122:
			return Z;
			break;		
	}
// AMD002 - END	
}

// AMD003 - START
// Clickable region functions ---------------------------------------------------------------------------
int check_clickableRect(position, cRect)
vector2 position;
clickable_rect *cRect;
{
    if (position.x >= cRect->topleft.x && position.y >= cRect->topleft.y)
    {
        if (position.x <= cRect->bottomright.x && position.y <= cRect->bottomright.y)
        {
            return 1;
        }
    }    
    return 0;
}
// AMD003 - END