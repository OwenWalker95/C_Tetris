// ow_display.h //
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
#ifndef OW_DISPLAY_H
#define OW_DISPLAY_H

//FUNCTIONS///////////////////////////////////////////////////////////////////////////////////////////////
// Window Functions --------------------------------------------------------------------------------------

// Creates either a full screen window (full = 1) or a window with size (width,height) (full =0). Name must be 30 chars
int create_window(int width, int height, char name[30], int full); 

// Returns vector2 of windowsize (w,h)
vector2 get_winSize();

// Resize the window to (width,height)
int resize_window(int width, int height);

// Move window to center of screen
int center_window();

// Move window to topleft of screen
int topleft_window();


// Renderer functions --------------------------------------------------------------------------------------
// Present the renderer
void present_renderBuffer();

// Fill window with a color
void draw_background(vector3 color);

// Darken current display within given rectangle by given darkness
void darkenRectangle(vector2 start, vector2 end, int darkness);

// Primitive shapes ---------------------------------------------------------------------------------------
// Draw a single point
void draw_point(vector2 position, vector3 color);

// Draw a line
// thickness = 0 is a 1 pixel wide line, each increment in thickness doubles the line around its center
void draw_line(vector2 start, vector2 end, vector3 color, int thickness);

// Draw a rectangle 
// thickness rules same as line
void draw_rectangle(vector2 start, vector2 end, vector3 color, int fill, int thickness);

// Draw a triangle
// thickness rules same as line
void draw_triangle(vector2 v1, vector2 v2, vector2 v3, vector3 color, int fill, int thickness);

// Draw a circle
// thickness = 0 is a 1 pixel wide circle, each increment in thickness draws another circle 1 pixel inside the original one
void draw_circle(vector2 center, int radius, vector3 color, int fill, int thickness);

// Text----------------------------------------------------------------------------------------------------------------
// Draw Text
// font must be a .ttf filename incl file extension
void draw_text(char text[],char font[], int size, vector3 color, vector2 position);  


// Image Rendering ------------------------------------------------------------------------------------------------------
// Renders png file, size is percent of original image size
void draw_png(char file[], int size, vector2 position);

#endif