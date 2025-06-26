// ow_util.h //
//
// general utils
//
// 01JAN2025 - Started
// 16MAR2025 - V1 Completed
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Current Changes -- V2
//
// AMD001: 12/05/2025 - Added function to times numbers by a power
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef OW_UTIL_H
#define OW_UTIL_H

// DATA STRUCTURES -----------------------------------------------------------------------------------------------
// VECTOR STRUCTS
struct v2
{
	int x;
	int y;
};
typedef struct v2 vector2;

struct v3
{
	int x;
	int y;
	int z;
};
typedef struct v3 vector3;
// ---------------------------------------------------------------------------------------------------------------


//FUNCTIONS////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector Functions ----------------------------------------------------------------------------------------------
// Returns a vector2 comprising of the entered x, y parameters
vector2 Vector2(int x, int y); 

// Currently just adds and subtracts v2 to/from v1
vector2 Vector2_math(vector2 v1, vector2 v2, char op); 

// Returns a vector3 comprising of the entered x, y, z parameters
vector3 Vector3(int x, int y, int z); 


// Random number generation ---------------------------------------------------------------------------------------
// Seed randon numer generator
void seed_rng();

// Return random int from range min - max inclusive of both
int get_randomRange(int min, int max);

// Sorting Functions ----------------------------------------------------------------------------------------------
// Swaps the values of 2 int pointers
int int_swap(int *int1, int *int2);

// Sort a list of int (up to 250000 of them)
// Provide int pointer for start of list, and list length. Updates the original list with sorted result
void sort_list(int *in_list, int list_len);

// AMD001 - S
int num_toPower(int num, int power);
// AMD001 - E

#endif