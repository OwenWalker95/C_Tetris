// ow_strings.c //
//
// string utils
//
// 02MAR2025 - Started
// 16MAR2025 - V1 Completed
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Current Changes -- V2
//
// AMD001: 
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// FUNCTIONS /////////////////////////////////////////////////////////////////////////////////////////////
// convert a string to a float value
// starts once it finds a number and ends once it reaches a non numeric char. ignores leading spaces but not other chars. 
double str_to_float(string)
char *string;
{
	// convert original string into working string
	size_t len = strlen(string); // original string length
	int digit; // current digit value
	
	// positions in original string
	int num_start = len; // index of first numeric char
	int num_end = len; // index of last numeric char
	int decimal = len; // index of decimal place (defaults to 1 greater than the highest index in case there isn't one)
	int negative = 0;
	
	// initial validation
	// find these positions
	for (int i = 0; i < len; i++)
	{
		// convert the char to numeric
		digit = string[i] - '0';
		// If numeric update the end point, if start point not set yet then set it, continue to next char
		if (digit < 10 && digit >= 0)
		{
			num_end = i;
			
			if (num_start == len)
			{
				num_start = i;
			}
			
			continue;
		}

		// if it's a '-' sign and number hasn't yet started, set negative flag
		if (num_start == len && string[i] == '-')
		{
			negative = 1;
			continue;
		}
		
		// if it's a space and not found a number yet, continue to next
		if (num_start == len && string[i] == ' ')
		{
			continue;
		}

		// If decimal hasn't been found yet, check if char is decimal point and if so save position and continue to next
		if (decimal == len && string[i] == '.')
		{
			decimal = i;
			continue;
		}
		
		break;
	}	
	
	// check if a number was actually found - if not, return 0
	if (num_start == len)
	{
		return 0.0;
	}
	
	int size_flag = 0;
	// make sure number is reasonable size_t
	if (decimal==len)
	{
		if (num_end - num_start > 9) {size_flag = 1;}
	}
	else
	{
		if (decimal - num_start > 9) {size_flag = 1;}
		if (num_end - decimal > 6) {size_flag = 1;}
	}
	
	if (size_flag == 1)
	{
		return 0;
	}
	
	// now go through number converting digits to true values and updating working value
	double working_val = 0.0; // working value
	for (int i = num_start; i <= num_end; i++)
	{
		digit = string[i] - '0';
		if (i == decimal) // ignore the decimal if there is one
		{
			continue;
		}
		
		// integers
		if (i < decimal)
		{
			int power;
			// find out which power of 10 it is
			if (decimal == len)
			{
				power = num_end - i;
			}
			else
			{
				power = decimal - 1 - i;
			}
			for (int powers = 0; powers < power; powers++)
			{
				digit = digit * 10;
			}
			working_val = working_val + (digit);
			continue;
		}
		
		// decimals
		if (i > decimal)
		{
			// which dp is it
			int dps = i - decimal;
			double dec_val = digit;
			
			
			for (int dp = 0; dp < dps; dp++)
			{
				dec_val = dec_val / 10.0;
			}
			working_val = working_val + (dec_val);
			continue;
		}
	}
	
	if (negative == 1)
	{
		working_val = working_val * -1;
	}
	return working_val;

}