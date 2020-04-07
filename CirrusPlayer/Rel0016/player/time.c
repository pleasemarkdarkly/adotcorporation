//****************************************************************************
//
// TIME.C - Functions for dealing with the system time.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#include "../hwport.h"
#include "../hwdefs.h"
#include "time.h"

//****************************************************************************
//
// The length of each month of the year, in days.  February is consider to be
// 29 days and the code explicitly handles making sure that it is 28 days in
// non-leap years.
//
//****************************************************************************
const unsigned char ucMonthLength[12] = { 31, 29, 31, 30, 31, 30,
                                          31, 31, 30, 31, 30, 31 };

//****************************************************************************
//
// TimeSetSystemTime programs the given time (specified in seconds since
// January 1, 1970) into the system real time clock.
//
//****************************************************************************
void
TimeSetSystemTime(unsigned long ulSeconds)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Program the given time into the EP7209's RTC.
    //
    pulPtr[HwRtcData >> 2] = ulSeconds;
}

//****************************************************************************
//
// TimeGetSystemTime returns the current value of the system real time clock
// (which is assumed to be the number of seconds since January 1, 1970).
//
//****************************************************************************
unsigned long
TimeGetSystemTime(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Return the current value of the EP7209's RTC.
    //
    return(pulPtr[HwRtcData >> 2]);
}

//****************************************************************************
//
// TimeFromSeconds converts the specified time in seconds since January 1,
// 1970 into a time structure, giving the corresponding month, day, year,
// hour, minute, second, and weekday.
//
//****************************************************************************
void
TimeFromSeconds(tTime *pTime, unsigned long ulSeconds)
{
    long lYear, lIdx = 0;

    //
    // Extract the number of seconds from the time.
    //
    pTime->cSec = ulSeconds % 60;
    ulSeconds /= 60;

    //
    // Extract the number of minutes from the time.
    //
    pTime->cMin = ulSeconds % 60;
    ulSeconds /= 60;

    //
    // Extract the number of hours from the time.
    //
    pTime->cHour = ulSeconds % 24;
    ulSeconds /= 24;

    //
    // We now have days, so add the number of days between January 1, 1900 and
    // January 1, 1970.
    //
    ulSeconds += (70 * 365) + 18;

    //
    // The day of the week just happens to work out this way.
    //
    pTime->cWeekday = ulSeconds % 7;

    //
    // Compute the number of years in terms of group of years from leap year to
    // leap year.
    //
    lYear = 4 * (ulSeconds / ((365 * 4) + 1));
    ulSeconds %= (365 * 4) + 1;

    //
    // If there are more than 365 days left in the current count of days, then
    // subtract the days from the remaining non-leap years.
    //
    if(ulSeconds >= 366)
    {
        lYear += (ulSeconds - 1) / 365;
        ulSeconds = (ulSeconds - 1) % 365;
    }

    //
    // Save the computed year.
    //
    pTime->sYear = 1900 + lYear;

    //
    // If this is a non-leap year and the day is past February 28th, then
    // increment the count of days by one (i.e. act as if each year is a leap
    // year).
    //
    if(((lYear & 3) != 0) && (ulSeconds >= (31 + 28)))
    {
        ulSeconds++;
    }

    //
    // Subtract days for each month till we find the current month.
    //
    while(ulSeconds >= ucMonthLength[lIdx])
    {
        ulSeconds -= ucMonthLength[lIdx++];
    }

    //
    // Save the computed month and day.
    //
    pTime->cDay = ulSeconds + 1;
    pTime->cMonth = lIdx + 1;
}

//****************************************************************************
//
// TimeToSeconds converts the specified time as month, day, year, hour,
// minute, and second into the number of seconds since January 1, 1970.
//
//****************************************************************************
void
TimeToSeconds(tTime *pTime, unsigned long *pulSeconds)
{
    long lIdx;

    //
    // Compute the number of days that have past in this year.
    //
    *pulSeconds = pTime->cDay - 1;
    lIdx = pTime->cMonth - 1;
    while(lIdx--)
    {
        *pulSeconds += ucMonthLength[lIdx];
    }

    //
    // If this is a non-leap year and the day is past February 28th, then
    // subtract the count of days by one (since we treat each year as if it
    // were a leap year).
    //
    if(((pTime->sYear & 3) != 0) && (*pulSeconds >= (31 + 28)))
    {
        (*pulSeconds)--;
    }

    //
    // Add the days for the years that have past.
    //
    lIdx = pTime->sYear - 1970;
    *pulSeconds += (lIdx * 365) + ((lIdx + 1) / 4);

    //
    // Convert the days to hours and add the current hour.
    //
    *pulSeconds = (*pulSeconds * 24) + pTime->cHour;

    //
    // Convert the hours to minutes and add the current minute.
    //
    *pulSeconds = (*pulSeconds * 60) + pTime->cMin;

    //
    // Convert the minutes to seconds and add the current second.
    //
    *pulSeconds = (*pulSeconds * 60) + pTime->cSec;
}

//****************************************************************************
//
// TimeComputeWeekday computes the weekday for the given time structure.
//
//****************************************************************************
static void
TimeComputeWeekday(tTime *pTime)
{
    unsigned long ulDays, ulIdx;

    //
    // Compute the number of days that have past in this year.
    //
    ulDays = pTime->cDay - 1;
    ulIdx = pTime->cMonth - 1;
    while(ulIdx--)
    {
        ulDays += ucMonthLength[ulIdx];
    }

    //
    // If this is a non-leap year and the day is past February 28th, then
    // subtract the count of days by one (since we treat each year as if it
    // were a leap year).
    //
    if(((pTime->sYear & 3) != 0) && (ulDays >= (31 + 28)))
    {
        ulDays--;
    }

    //
    // Add the days for the years that have past.
    //
    ulIdx = pTime->sYear - 1970;
    ulDays += (ulIdx * 365) + ((ulIdx + 1) / 4) + ((70 * 365) + 18);

    //
    // Compute the day of the week.
    //
    pTime->cWeekday = ulDays % 7;
}

//****************************************************************************
//
// TimeIncrementMonth is used to increment the month by one.
//
//****************************************************************************
void
TimeIncrementMonth(tTime *pTime)
{
    //
    // Increment the month, wrapping back to January if necessary.
    //
    if(++pTime->cMonth == 13)
    {
        pTime->cMonth = 1;
    }

    //
    // Adjust the day if it is invalid for the new month.
    //
    if(pTime->cDay > ucMonthLength[pTime->cMonth - 1])
    {
        pTime->cDay = ucMonthLength[pTime->cMonth - 1];
    }
    if(((pTime->sYear & 3) != 0) && (pTime->cMonth == 2) && (pTime->cDay > 28))
    {
        pTime->cDay = 28;
    }

    //
    // Update the weekday.
    //
    TimeComputeWeekday(pTime);
}

//****************************************************************************
//
// TimeDecrementMonth is used to decrement the month by one.
//
//****************************************************************************
void
TimeDecrementMonth(tTime *pTime)
{
    //
    // Decrement the month, wrapping back to January if necessary.
    //
    if(--pTime->cMonth == 0)
    {
        pTime->cMonth = 12;
    }

    //
    // Adjust the day if it is invalid for the new month.
    //
    if(pTime->cDay > ucMonthLength[pTime->cMonth - 1])
    {
        pTime->cDay = ucMonthLength[pTime->cMonth - 1];
    }
    if(((pTime->sYear & 3) != 0) && (pTime->cMonth == 2) && (pTime->cDay > 28))
    {
        pTime->cDay = 28;
    }

    //
    // Update the weekday.
    //
    TimeComputeWeekday(pTime);
}

//****************************************************************************
//
// TimeIncrementDay is used to increment the day by one.
//
//****************************************************************************
void
TimeIncrementDay(tTime *pTime)
{
    //
    // Increment the day.
    //
    pTime->cDay++;

    //
    // Adjust the day back to the first if we are past the end of the month.
    //
    if((pTime->cDay > ucMonthLength[pTime->cMonth - 1]) ||
       (((pTime->sYear & 3) != 0) && (pTime->cMonth == 2) &&
        (pTime->cDay > 28)))
    {
        pTime->cDay = 1;
    }

    //
    // Update the weekday.
    //
    TimeComputeWeekday(pTime);
}

//****************************************************************************
//
// TimeDecrementDay is used to decrement the day by one.
//
//****************************************************************************
void
TimeDecrementDay(tTime *pTime)
{
    //
    // Decrement the day.
    //
    pTime->cDay--;

    //
    // Adjust the day up to the end of the month if we are before the beginning
    // of the month.
    //
    if(pTime->cDay == 0)
    {
        if(((pTime->sYear & 3) != 0) && (pTime->cMonth == 2))
        {
            pTime->cDay = 29;
        }
        else
        {
            pTime->cDay = ucMonthLength[pTime->cMonth - 1];
        }
    }

    //
    // Update the weekday.
    //
    TimeComputeWeekday(pTime);
}

//****************************************************************************
//
// TimeIncrementYear is used to increment the year by one.
//
//****************************************************************************
void
TimeIncrementYear(tTime *pTime)
{
    //
    // Increment the year if it is less than 2099.
    //
    if(pTime->sYear < 2099)
    {
        pTime->sYear++;
    }

    //
    // Update the weekday.
    //
    TimeComputeWeekday(pTime);
}

//****************************************************************************
//
// TimeDecrementYear is used to decrement the year by one.
//
//****************************************************************************
void
TimeDecrementYear(tTime *pTime)
{
    //
    // Decrement the year if it is greater than 1970.
    //
    if(pTime->sYear > 1970)
    {
        pTime->sYear--;
    }

    //
    // Update the weekday.
    //
    TimeComputeWeekday(pTime);
}

//****************************************************************************
//
// TimeIncrementHour is used to increment the hour by one.
//
//****************************************************************************
void
TimeIncrementHour(tTime *pTime)
{
    //
    // Increment the hour by one, wrapping back to zero if necessary.
    //
    pTime->cHour++;
    if(pTime->cHour == 24)
    {
        pTime->cHour = 0;
    }
}

//****************************************************************************
//
// TimeDecrementHour is used to decrement the hour by one.
//
//****************************************************************************
void
TimeDecrementHour(tTime *pTime)
{
    //
    // Decrement the hour by one, wrapping back to 23 if necessary.
    //
    pTime->cHour--;
    if(pTime->cHour == (char)-1)
    {
        pTime->cHour = 23;
    }
}

//****************************************************************************
//
// TimeIncrementMinute is used to increment the minute by one.
//
//****************************************************************************
void
TimeIncrementMinute(tTime *pTime)
{
    //
    // Increment the minute by one, wrapping back to zero if necessary.
    //
    pTime->cMin++;
    if(pTime->cMin == 60)
    {
        pTime->cMin = 0;
    }
}

//****************************************************************************
//
// TimeDecrementMinute is used to decrement the minute by one.
//
//****************************************************************************
void
TimeDecrementMinute(tTime *pTime)
{
    //
    // Decrement the minute by one, wrapping back to 59 if necessary.
    //
    pTime->cMin--;
    if(pTime->cMin == (char)-1)
    {
        pTime->cMin = 59;
    }
}

//****************************************************************************
//
// TimeIncrementSecond is used to increment the second by one.
//
//****************************************************************************
void
TimeIncrementSecond(tTime *pTime)
{
    //
    // Increment the second by one, wrapping back to zero if necessary.
    //
    pTime->cSec++;
    if(pTime->cSec == 60)
    {
        pTime->cSec = 0;
    }
}

//****************************************************************************
//
// TimeDecrementSecond is used to decrement the second by one.
//
//****************************************************************************
void
TimeDecrementSecond(tTime *pTime)
{
    //
    // Decrement the second by one, wrapping back to 59 if necessary.
    //
    pTime->cSec--;
    if(pTime->cSec == (char)-1)
    {
        pTime->cSec = 59;
    }
}
