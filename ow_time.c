// ow_time.c //
//
// general time functions
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
#include <stdio.h>
#include <time.h>

#include "ow_time.h"
// Data structure for use by timer functions--------------------------------------------------------------
struct ow_timer
{
	long sec;
	int mil;
};
static struct ow_timer ow_Timer;
//--------------------------------------------------------------------------------------------------------

//FUNCTIONS//////////////////////////////////////////////////////////////////////////////////////////////
// Date/Time functions ----------------------------------------------------------------------------
// Sets date & timestamp to a second, each unit stored seperately
ow_timestamp get_dateTime()
{
    time_t current_time = time(NULL);
	
	if (current_time == -1)
	{
		printf("time failed");
	}
	
	struct tm *local_time = localtime(&current_time);
	ow_timestamp timestamp;
	
	if (local_time == NULL)
	{
		printf("time convert failed");
	}
	
	timestamp.day = local_time->tm_mday; //day of month 1-31
	timestamp.month = local_time->tm_mon + 1; // month (0-11 add 1 for 1-12)
	timestamp.year = local_time->tm_year + 1900; // years since 1900
	timestamp.hour = local_time->tm_hour; // hours 0-23
	timestamp.minute = local_time->tm_min; // minutes 0 - 59
	timestamp.second = local_time->tm_sec; // seconds 0 - 59

	return timestamp;
}

ow_date get_date()
{
    time_t current_time = time(NULL);
	
	if (current_time == -1)
	{
		printf("time failed");
	}
	
	struct tm *local_time = localtime(&current_time);
	ow_date datestamp;
	
	if (local_time == NULL)
	{
		printf("time convert failed");
	}
	
	datestamp.day = local_time->tm_mday; //day of month 1-31
	datestamp.month = local_time->tm_mon + 1; // month (0-11 add 1 for 1-12)
	datestamp.year = local_time->tm_year + 1900; // years since 1900

	return datestamp;	
}

// Timer functions ---------------------------------------------------------------------------------
void start_timer()
{
	struct timespec ts;
	
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
	{
		printf("get time failed\n");
		return;
	}
	
	long seconds = ts.tv_sec; // seconds since the unix epoch
	long nanoseconds = ts.tv_nsec; // nanoseconds past the current second

	int milliseconds = nanoseconds / 1000000;
	
	ow_Timer.sec = seconds;
	ow_Timer.mil = milliseconds;
}

int get_timer()
{
	struct timespec ts;
	
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
	{
		printf("get time failed\n");
		return 0;
	}
	
	long seconds = ts.tv_sec; // seconds since the unix epoch
	long nanoseconds = ts.tv_nsec; // nanoseconds past the current second
	int milliseconds = (nanoseconds / 1000000);

    // how many full seconds since start
	int seconds_passed = seconds - ow_Timer.sec;
	int mils_passed = milliseconds - ow_Timer.mil;
	
	if (mils_passed < 0)
	{
		seconds_passed = seconds_passed - 1;
		mils_passed = mils_passed + 1000;
	}
	
	// total milliseconds
	mils_passed = mils_passed + (seconds_passed * 1000);
	
	return mils_passed;
}

void wait(wait_ms)
int wait_ms;
{
	void start_timer();	
	int start_t = get_timer();
	int ms = 0;
	while (ms - start_t < wait_ms)
	{
		ms = get_timer();
	}
}

