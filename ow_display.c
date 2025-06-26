// ow_display.c //
//
// general display functions
//
// 01JAN2025 - Started
// 16MAR2025 - V1 Completed
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Current Changes -- V2
//
// AMD001: 
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//INCLUDES
// General C includes 
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "ow_util.h"
//////////////////////////////////////////
// Static SDL structures for window creation and rendering -------------------------------------------------
static SDL_Window *window;
static SDL_Renderer *renderer;

// 99% sure this is redundant, check during V2 dev
struct window_data
{
	int w;
	int h;
};
static struct window_data ow_window;

struct text_struct
{		
    TTF_Font* font;
    SDL_Surface* surface; 	
    SDL_Texture* texture;
	SDL_Rect rect;
};
static struct text_struct text_object;

//FUNCTIONS/////////////////////////////////
// Window Functions --------------------------------------------------------------------------------------
int create_window(width, height, name, full)
int width, height, full;
char name[30];
{

	// Input checking
	if (full != 0 && full != 1)
	{
		printf("Error - in create_window(.,.,.,!) - invalid value for 'full', use 0, 1 or 2 - aborted\n");
		return 1;
	}
	
	// Initialise SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0) 
	{
        printf("TTF_Init Error: %s\n", SDL_GetError());
        return 1;
    }
	
	// Create Window
	if (full == 1)
	{		
		window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);

	}
	else 
	{
		window	= SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,width, height, SDL_WINDOW_SHOWN);	
	}
	
    if (window == NULL) 
	{
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) 
	{
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
	
	SDL_GetWindowSize(window, &ow_window.w, &ow_window.h);
}

int resize_window(width, height)
int width;
int height;
{
	SDL_SetWindowSize(window, width, height);
}

int center_window()
{
	SDL_SetWindowPosition(window,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED);
}

int topleft_window()
{
	SDL_SetWindowPosition(window,0,0);
}

vector2 get_winSize()
{
	vector2 result;
	SDL_GetWindowSize(window, &result.x, &result.y);
	return result;
}
// Renderer fuctions --------------------------------------------------------------------------------------
void present_renderBuffer()
{
	SDL_RenderPresent(renderer);	
}

void draw_background(color) 
vector3 color; 
{
	if (color.x < 0 || color.y < 0 || color.z < 0)
	{
		printf("error - in draw_background(!) - invalid negative color values - aborted\n");
		return;
	}
	
	if (color.x > 255 || color.y > 255 || color.z > 255)
	{
		printf("error - in draw_background(!) - color value exceeded maximum - aborted\n");
		return;
	}	
	
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255);	
    SDL_RenderClear(renderer);	
	present_renderBuffer();
}

void darkenRectangle(start, end, darkness)
vector2 start, end;
int  darkness;
{
	int w,h;
	w = end.x - start.x;
	h = end.y - start.y;
	
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, darkness);
	SDL_Rect area = {start.x, start.y, w, h};
	SDL_RenderFillRect(renderer, &area);
	
}

// Primitive shapes ---------------------------------------------------------------------------------------

void draw_point(position, color)
vector2 position;
vector3 color;
{
	SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255);			
	SDL_RenderDrawPoint(renderer, position.x, position.y);	
}

void draw_lineSingle(start, end, color)
vector2 start;
vector2 end;
vector3 color;
{	  
	// input checks - fatal errors
    if (start.x < 0 || start.y < 0 || end.x < 0 || end.y < 0)
	{
		printf("error - in draw_lineSingle(!,!,.) - invalid negative coordinates - aborted\n");
		return;
	}
	if (start.x == end.x && start.y == end.y)
	{
//		printf("error - in draw_lineSingle(!,!,.) - start and end points cannot be same - aborted\n");
		return;
	}
	if (color.x < 0 || color.y < 0 || color.z < 0)
	{
		printf("error - in draw_lineSingle(.,.,!) - invalid negative color values - aborted\n");
		return;
	}
	if (color.x > 255 || color.y > 255 || color.z > 255)
	{
		printf("error - in draw_lineSingle(.,.,!) - color value exceeded maximum - aborted\n");
		return;
	}
	// input checks - warnings
	if (start.x > ow_window.w || start.y > ow_window.h || end.x > ow_window.w || end.y > ow_window.h)
	{
		printf("warning - in draw_lineSingle(?,?,.) - start or end position outside window - ignored\n");
	}
	
	// Ensure always starting from left (lowest x) unless vertical line then start from top (lowest y)
	vector2 swap_point; // use for switching start + end point
	if (start.x == end.x && start.y > end.y) // vertical and starting from bottom so switch
	{
		swap_point = start;
		start = end;
		end = swap_point;
	}
	else if (start.x > end.x) // horizontal and starting from right so switch
	{
		swap_point = start;
		start = end;
		end = swap_point;
	}
	
	// calculate gradient and y intercept for use in line equation for diagonals	
	float x_diff = end.x - start.x;	
	float y_diff = end.y - start.y;

	float gradient;	
	if (x_diff == 0)
	{
		gradient = 0;
	}
	else
	{
		gradient = y_diff / x_diff;
	}

	float y_intercept;
	y_intercept = start.y - (start.x * gradient);
	
	// how long is the line along the axis of movement, this is x axis unless a vertical line
	int steps; 
	if (x_diff == 0)
	{
		steps = y_diff;
	}
	else
	{
		steps = x_diff;
	}
	
	// draw the line	
	vector2 current_point = start;	
	int step;
	int next_y;
	for (step = 0; step< steps; step++)
	{
		// draw current point
		//SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255);			
		//SDL_RenderDrawPoint(renderer, current_point.x, current_point.y);
		draw_point(current_point, color);

		if (x_diff == 0)
		{
			// vertical line so just move current point along y axis
			current_point.y = current_point.y + 1;
			// can i use a 'continue' here so i can un-nest the following code??
			// everything after this point is test code, replacing the commented out stuff below
			continue;
		}
		
		// move current point along x axis
		current_point.x = current_point.x + 1;
		
		if (y_diff == 0)
		{
			continue;
		}
		
		// line is diagonal so calculate the correct y value for current x value
		next_y = (gradient * current_point.x) + y_intercept;
		
		// move current point along the y axis until reaching the correct position, drawing each point along the way
		if (y_diff > 0)
		{
			for (current_point.y = current_point.y; current_point.y < next_y; current_point.y++)
			{
				draw_point(current_point, color);
			}
		}
		else 
		{
			for (current_point.y = current_point.y; current_point.y > next_y; current_point.y--)
			{
				draw_point(current_point, color);
			}					
		}	
	}
}

void draw_line(start, end, color, thickness)
vector2 start;
vector2 end;
vector3 color;
int thickness;
{
	vector2 newStart;
	vector2 newEnd;
	
	draw_lineSingle(start, end, color);
	// draw a line either side of the central line for each step in thickness
	int i;
	for (i = 1; i <= thickness; i++)
	{
		// is line vertical
		if (start.x == end.x)
		{
			newStart = Vector2_math(start,Vector2(i,0),'+');
			newEnd = Vector2_math(end,Vector2(i,0),'+');
			draw_lineSingle(newStart,newEnd,color);
			
			newStart = Vector2_math(start,Vector2(i,0),'-');
			newEnd = Vector2_math(end,Vector2(i,0),'-');
			draw_lineSingle(newStart,newEnd,color);			
		}
		else
		{
			newStart = Vector2_math(start,Vector2(0,i),'+');
			newEnd = Vector2_math(end,Vector2(0,i),'+');
			draw_lineSingle(newStart,newEnd,color);			
			
			newStart = Vector2_math(start,Vector2(0,i),'-');
			newEnd = Vector2_math(end,Vector2(0,i),'-');
			draw_lineSingle(newStart,newEnd,color);						
		}
		
	}
}

void draw_rectangle(start, end, color, fill, thickness)
vector2 start;
vector2 end;
vector3 color;
int fill;
int thickness;
{
	// ensure always go top left to bottom right
	
	if (start.x > end.x)
	{
		int_swap(&start.x,&end.x);
	}
	if (start.y > end.y)
	{
		int_swap(&start.y, &end.y);
	}
	
	if (fill == 0)
	{
		draw_line(start,Vector2(end.x,start.y), color, thickness);
		draw_line(start,Vector2(start.x,end.y), color, thickness);
		draw_line(Vector2(end.x,start.y), end, color, thickness);
		draw_line(Vector2(start.x,end.y), end, color, thickness);	
		return;
	}
	
	int curr_y = start.y;
	int y_diff;
    
	
	for (curr_y = start.y; curr_y <= end.y; curr_y++)
	{
		draw_lineSingle(Vector2(start.x,curr_y),Vector2(end.x,curr_y), color);			
	}
}
	
void draw_triangle(v1, v2, v3, color, fill, thickness)
vector2 v1, v2, v3;
vector3  color;
int fill;
int thickness;	
{
	vector2 start_v = v1;
	vector2 end_v1 = v2;
	vector2 end_v2 = v3;
	//start from furthest left corner
	vector2 swap_v2;
	if (end_v1.x < start_v.x)
	{
		swap_v2 = start_v;
		start_v = end_v1;
		end_v1 = swap_v2;
	}
	if(end_v2.x < start_v.x)
	{
		swap_v2 = start_v;
		start_v = end_v2;
		end_v2 = swap_v2;
	}
	
	// ensure 2 other corners go from left to right
	if (end_v2.x < end_v1.x)
	{
		swap_v2 = end_v1;
		end_v1 = end_v2;
		end_v2 = swap_v2;
	}
	
	if (fill == 0)
	{
		draw_line(start_v,end_v1, color, thickness);
		draw_line(start_v,end_v2, color, thickness);
		draw_line(end_v1, end_v2, color, thickness);
		return;
	}
	
	//join the start point to every point along the last line
	
	
	// calculate gradient and y intercept for use in line equation for diagonals	
	float x_diff = end_v2.x - end_v1.x;	
	float y_diff = end_v2.y - end_v1.y;

	float gradient;	
	if (x_diff == 0)
	{
		gradient = 0;
	}
	else
	{
		gradient = y_diff / x_diff;
	}

	float y_intercept;
	y_intercept = end_v1.y - (end_v1.x * gradient);
	
	// how long is the line along the axis of movement, this is x axis unless a vertical line
	int steps; 
	if (x_diff == 0)
	{
		steps = y_diff;
	}
	else
	{
		steps = x_diff;
	}	
	vector2 current_point = end_v1;	
	int step;
	int next_y;
	for (step = 0; step< steps; step++)
	{
		// draw line from start to current point
		//SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255);			
		//SDL_RenderDrawPoint(renderer, current_point.x, current_point.y);
		draw_lineSingle(start_v, current_point, color);

		if (x_diff == 0)
		{
			// vertical line so just move current point along y axis
			current_point.y = current_point.y + 1;
			// can i use a 'continue' here so i can un-nest the following code??
			// everything after this point is test code, replacing the commented out stuff below
			continue;
		}
		
		// move current point along x axis
		current_point.x = current_point.x + 1;
		
		if (y_diff == 0)
		{
			continue;
		}
		
		// line is diagonal so calculate the correct y value for current x value
		next_y = (gradient * current_point.x) + y_intercept;
		
		// move current point along the y axis until reaching the correct position, drawing each point along the way
		if (y_diff > 0)
		{
			for (current_point.y = current_point.y; current_point.y < next_y; current_point.y++)
			{
				draw_lineSingle(start_v,current_point, color);
			}
		}
		else 
		{
			for (current_point.y = current_point.y; current_point.y > next_y; current_point.y--)
			{
				draw_lineSingle(start_v, current_point, color);
			}					
		}	
	}	
}
	
void draw_circleSingle(center, radius, color, fill)
vector2 center;
int radius;
vector3 color;
int fill;
{
	int i; 
	int x_dist, y_dist, total_dist;
	int draw_x, draw_y;
	int last_y, curr_y;
	last_y = center.y;
	curr_y = center.y;
	for (i = center.x - radius; i <= center.x; i++)
	{
		x_dist = center.x - i;		
		for (y_dist = 0; y_dist <= radius; y_dist++)
		{
			
			total_dist = (x_dist * x_dist) + (y_dist * y_dist);	
			if (total_dist > (radius * radius))
			{
				break;
			}
		}
		
		if (fill == 1)
		{

			draw_lineSingle(Vector2(i,center.y+y_dist), Vector2(i,center.y-y_dist),color);
			draw_lineSingle(Vector2((center.x+radius) - (radius - x_dist),center.y+y_dist), Vector2((center.x+radius) - (radius - x_dist),center.y-y_dist),color);			
		}
		else
		{
			if (center.y + y_dist > last_y)
			{
				for (curr_y = last_y; curr_y <= y_dist; curr_y++)
				{
					draw_point(Vector2(i,center.y - curr_y), color); // left upper
					draw_point(Vector2((center.x+radius) - (radius - x_dist), center.y - curr_y), color); //right upper
					
					draw_point(Vector2(i, center.y + curr_y), color); // left lower
					draw_point(Vector2((center.x+radius) - (radius - x_dist),center.y + curr_y), color); //right lower					
				}
			}
			
			draw_point(Vector2(i,center.y - y_dist), color); // left upper
			draw_point(Vector2((center.x+radius) - (radius - x_dist), center.y - y_dist), color); //right upper
			
			draw_point(Vector2(i, center.y + y_dist), color); // left lower
			draw_point(Vector2((center.x+radius) - (radius - x_dist),center.y + y_dist), color); //right lower	

			last_y = y_dist;
		}	
	}	
}	

void draw_circle(center, radius, color, fill, thickness)
vector2 center;
int radius;
vector3 color;
int fill, thickness;
{
	if (fill == 1)
	{
		draw_circleSingle(center,radius, color, fill);
		return;
	}
	
	int i;
	for (i = 0; i <= thickness; i++)
	{
		draw_circleSingle(center, radius, color, fill);
		radius = radius -1;
	}
}
	
// Text Functions --------------------------------------------------------------------------------------	
void draw_text(text, font, size, color, position)
char text[];
char font[];
int size;
vector3 color;
vector2 position;
{
	text_object.font = TTF_OpenFont(font, size);
	SDL_Color col = {color.x, color.y, color.z, 255};
	
	text_object.surface = TTF_RenderText_Solid(text_object.font, text, col); 	
	text_object.texture = SDL_CreateTextureFromSurface(renderer, text_object.surface);
	SDL_FreeSurface(text_object.surface);	
	
	// find position
	int x, y;
	x = position.x;
	y = position.y;
	
	int width, height;
	
	SDL_QueryTexture(text_object.texture, NULL, NULL, &width, &height);
	
	text_object.rect.x = x;
	text_object.rect.y = y;
	text_object.rect.w = width;
	text_object.rect.h = height;
	
    SDL_RenderCopy(renderer, text_object.texture, NULL, &text_object.rect);	
	
    present_renderBuffer();
}
	
// Image Rendering -------------------------------------------------------------------------------------
void draw_png(file, size, position)
char file[];
int size;
vector2 position;
{
	SDL_Texture* texture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(file);
	if (!loadedSurface)
	{
		printf("Failed to load image: %s, SDL_image Error: %s\n", file, IMG_GetError());
		return;
	}
	
	texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
	SDL_FreeSurface(loadedSurface);
	
	int width, height;
	SDL_QueryTexture(texture, NULL, NULL, &width, &height);
	
	printf("width=%d, height = %d\n", width, height);
	
	int x, y;
	x = position.x;
	y = position.y;
	
	float img_w, img_h;
	
	img_w = ((float)width / 100) * size;
	img_h = ((float)height / 100) * size;
	
	width = img_w;
	height = img_h;
	
	printf("img_w = %f, img_h = %f\n-----------------\n", img_w, img_h);
	
	SDL_Rect img_rect;
	img_rect.x = x;
	img_rect.y = y;
	img_rect.w = width;
	img_rect.h = height;
	
	SDL_RenderCopy(renderer, texture, NULL, &img_rect);
}	
	

