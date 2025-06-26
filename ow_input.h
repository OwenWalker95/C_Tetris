// ow_input.h //
//
// general input functions
//
// 25JAN2025 - Started
// 16MAR2025 - V1 Completed
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Current Changes -- V2
//
// AMD001: 28/03/25 - Added diagnostic function to print numeric key identifier
// AMD002: 28/03/25 - Added more keys to the ow_keyButtons enum
// AMD003: 15/03/25 - Added data structure for clickable rectangle region
// AMD003: 21/06/25 - New function to check if position is within clickable rect
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef OW_INPUT_H
#define OW_INPUT_H

#include "ow_util.h"

// Data Structures ----------------------------------------------------------------------------------------
// Input types - returned by get_input()
enum ow_input_enum
{
	NONE,
	QUIT,
	KEYPRESS_DOWN,
	LEFT_CLICK_DOWN,
	RIGHT_CLICK_DOWN,
	MOUSE_MOVED
	
};
typedef enum ow_input_enum ow_input_type;


// Input Key Names
enum ow_keyButtons_enum
{
// AMD002 - START	
//	ESCAPE
	ESCAPE,
	UP_ARROW,
	DOWN_ARROW,
	LEFT_ARROW,
	RIGHT_ARROW,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z
// AMD002 - END
};
typedef enum ow_keyButtons_enum ow_keyButton; 
// -------------------------------------------------------------------------------------------------------

// AMD003 - START
// Clickable region (rectangular) which can be enabled or disabled
struct clickable_Rect
{
    vector2 topleft;
    vector2 bottomright;
    int active; // 0 = no. 1 = yes.
};
typedef struct clickable_Rect clickable_rect;
// AMD003 - END

//FUNCTIONS///////////////////////////////////////////////////////////////////////////////////////////////

// Terminal functions -----------------------------------------------------------------------------------
// Gets input from cli up to 100 chars
void get_CLI(char *cli, int size);

// General functions ----------------------------------------------------------------------------------
// Clears the input buffer / queue
void clear_input();
// Returns latest input type
ow_input_type get_input();

// Mouse functions ------------------------------------------------------------------------------------
// Returns position of last mouse click
vector2 get_mouseClickPosition();
vector2 get_mouseMovePosition();

// Keyboard functions -----------------------------------------------------------------------------------
// AMD001 - START
// Diagnostic function to print numeric key identifier to terminal
void print_keyNum();
// AMD001 - END
// Returns key that was pressed
ow_keyButton get_keyButton();

// AMD004 - START
// Clickable Regions Functions --------------------------------------------------------------------------
// Returns 1 if position is within bounds of clickable rect. else return 0
int check_clickableRect(vector2 position, clickable_rect *cRect);
// AMD004 - END

#endif