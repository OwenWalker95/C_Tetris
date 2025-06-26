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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ow_util.h"
#include "ow_time.h"

//FUNCTIONS//////////////////////////////////////////////////////////////////////////////////////////////
// Vector Functions -------------------------------------------------------------------------------------
vector2 Vector2(x, y)
int x, y;
{
	vector2 result;
	result.x = x;
	result.y = y;
	return result;
}

vector2 Vector2_math(v1, v2, op)
vector2 v1, v2;
char op;
{
	vector2 result;
	switch (op)
	{
		case '-':
			result.x = v1.x - v2.x;
			result.y = v1.y - v2.y;
			break;
		case '+':
			result.x = v1.x + v2.x;
			result.y = v1.y + v2.y;
			break;
	}
	
	return result;
}

vector3 Vector3(x, y, z)
int x, y, z;
{
	vector3 result;
	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}

// Random number generation ------------------------------------------------------------------------------
// Seed random number generator
void seed_rng()
{
	srand(time(NULL));
}

// Get random number in range
int get_randomRange(min, max)
int min, max;
{
	 return min + (rand() % (max - min + 1));
}

// Sorting Functions- ------------------------------------------------------------------------------------
// Swap the values of 2 ints
int int_swap(int1, int2)
int *int1, *int2;
{
	int swap_int = *int1;
	*int1 = *int2;
	*int2 = swap_int;
	
}

// Sort a list of int (up to 250000 of them)
void sort_list(in_list, list_len)
int *in_list;
int list_len;
{
	// needs testing i spose
	if (list_len > 250000)
	{
		return;
	}
	
	int passes = 0;
	int all_srtd = 0;		
	
	int solved[list_len]; // 0/1 flag
	for (int i = 0; i < list_len; i++)
	{
		solved[i] = 0;
	}
	int solve_count = 0;
	
	int list_flag = 0; // which list to use for current working one	
	int list_a[list_len];
	int list_b[list_len];
	int list_c[list_len];
		
	while (all_srtd == 0)
	// keep looping through the list until solved	
	{
		//initialise the pass
		int lowest[list_len];   // stack of lowest found numbers in a pass
		int lows = 0; // keep track of how many in stack
	
		int highest[list_len]; // same as lowest
		int highs = 0;	

		for (int i = 0; i < list_len; i++) 
		{		
			if (solved[i] == 1) // already selected this 1
			{
				continue;
			}		
			if (lows == 0 && highs == 0) // 1st number of the pass
			{	
				highest[0] = in_list[i];
				lowest[0] = in_list[i];
				solved[i] = 1;
				solve_count++;
				highs++;
				lows++;
				continue;
			}
			if (in_list[i] <= lowest[lows-1]) // lowest number is the latest one added, if there is 1 lowest then index = 0
			{
				lowest[lows] = in_list[i];
				lows++;
				solved[i] = 1;
				solve_count++;
				continue; // if it's lower than lowest it can't also be higher than highest
			}
			if (in_list[i] > highest[highs-1])
			{
				highest[highs] = in_list[i];
				highs++;
				solved[i] = 1;
				solve_count++;
				continue;
			}
		}
		// end of pass, merge highs and lows into working list		
		int *working_list;
		int *last_list; 
		int *new_list;
		switch (list_flag)
		{
			case 0:
				working_list = list_a;
				last_list = list_c;
				new_list = list_b;
				break;
			case 1:
				working_list = list_b;
				last_list = list_a;
				new_list = list_c;
				break;			
			case 2:
				working_list = list_c;
				last_list = list_b;
				new_list = list_a;
				break;			
		}		
		// populate wking list
		list_flag++;
		if (list_flag > 2)
		{
			list_flag = 0;
		}				
		for (int i = 0; i < lows; i++)
		{
			working_list[i] = lowest[(lows-1) - i];
		}		
		for (int i = 1; i < highs; i++) // omit the 1st one as its also the 1st in lowest
		{
			working_list[(i + lows) - 1] = highest[i];
		}
		if (passes == 0)
		{
			passes++;
			continue; // done this pass
		}
		//merge working list, and list from last pass into 3rd list
		int curr_list = 0;
		int work_index = 0;
		int last_index = 0;
		for (int i = 0; i < solve_count; i++) 
		{			
			if (work_index >= (highs + lows) -1) // ran out of numbers from this pass so just keep adding from the last list
			{
				new_list[i] = last_list[last_index];
				last_index++;
				continue;
			}			
			if (last_index > (solve_count - (highs + lows))) // ran out of last list so add from current pass
			{
				new_list[i] = working_list[work_index];
				work_index++;
				continue;
			}			
			if (working_list[work_index] <= last_list[last_index])
			{
				new_list[i] = working_list[work_index];
				work_index++;
			}
			else
			{
				new_list[i] = last_list[last_index];
				last_index++;
			}
		}
		// check if done
		if (solve_count == list_len)
		{
			for (int i = 0; i < solve_count; i++)
			{
				in_list[i] = new_list[i];
			}		
			all_srtd = 1;
			break;
		}
		// put new_list into work_list so next pass can use it
		for (int i = 0; i < solve_count; i++)
		{
			working_list[i] = new_list[i];
		}				
	}
	return;
}

// AMD001 - S
int num_toPower(num, power)
int num, power;
{
	int result = num;
	for (int i = 0; i < power-1; i++)
	{
		result = result*num;
	}
	return result;
}
// AMD001 - E