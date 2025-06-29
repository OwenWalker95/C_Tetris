// ow_strings.h //
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
#ifndef OW_STRINGS_H
#define OW_STRINGS_H


// FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////////
// convert a string to a float value
// starts once it finds a number and ends once it reaches a non numeric char. ignores leading spaces but not other chars. 
double str_to_float(char *string);
#endif