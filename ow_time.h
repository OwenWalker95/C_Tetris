// ow_time.h //
//
// general display functions
//
// 26JAN2025 - Started
// 16MAR2025 - V1 Completed
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Current Changes -- V2
//
// AMD001: 
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef OW_TIME_H
#define OW_TIME_H


// DATA STRUCTURES -----------------------------------------------------------------------------------------
// Full timestamp: dd.mm.yy.hh.mm.ss
struct OW_timestamp
{
	int day;
	int month;
	int year;
	int hour;
	int minute;
	int second;
};
typedef struct OW_timestamp ow_timestamp;

// Date only, dd.mm.yy
struct OW_date
{
	int day;
	int month;
	int year;
};
typedef struct OW_date ow_date;
//----------------------------------------------------------------------------------------------------------

//FUNCTIONS//////////////////////////////////////////////////////////////////////////////////////
// Date/Time Functions---------------------------------------------------------------------------
// Returns a populated ow_timestamp
ow_timestamp get_dateTime(); 

// Returns a populated ow_date
ow_date get_date();

// Timer functions-------------------------------------------------------------------------------
// Saves the current time in seconds and milliseconds
void start_timer();

// returns an int with the number of milliseconds passed since start_timer() last ran
int get_timer();

// returns after number of miliseconds (wait_ms) has passed
void wait(int wait_ms);


#endif