//****************************************************************************
//
// TIME.H - Definitions and function prototypes for time.c.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

//****************************************************************************
//
// The following structure contains a time broken down into it's component
// pieces (i.e. month, day, year, hour, minute, second, and day of week).
//
//****************************************************************************
typedef struct
{
    short sYear;
    char cMonth;
    char cDay;
    char cHour;
    char cMin;
    char cSec;
    char cWeekday;
} tTime;

//****************************************************************************
//
// Function prototypes.
//
//****************************************************************************
extern void TimeSetSystemTime(unsigned long ulSeconds);
extern unsigned long TimeGetSystemTime(void);
extern void TimeFromSeconds(tTime *pTime, unsigned long ulSeconds);
extern void TimeToSeconds(tTime *pTime, unsigned long *pulSeconds);
extern void TimeIncrementMonth(tTime *pTime);
extern void TimeDecrementMonth(tTime *pTime);
extern void TimeIncrementDay(tTime *pTime);
extern void TimeDecrementDay(tTime *pTime);
extern void TimeIncrementYear(tTime *pTime);
extern void TimeDecrementYear(tTime *pTime);
extern void TimeIncrementHour(tTime *pTime);
extern void TimeDecrementHour(tTime *pTime);
extern void TimeIncrementMinute(tTime *pTime);
extern void TimeDecrementMinute(tTime *pTime);
extern void TimeIncrementSecond(tTime *pTime);
extern void TimeDecrementSecond(tTime *pTime);
