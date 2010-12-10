#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "induction.h"
#include "timeutil.h"

int i_time_check_past (struct timeval *now, struct timeval *control)				/* Is now past then... if so, then a 1 is returned */
{
	if (now->tv_sec > control->tv_sec) return 1;						/* Control time has been past by seconds */
	if (now->tv_sec < control->tv_sec) return 0;						/* Control time hasnt been reached yet */

	/* At this point now->tv_sec and control->tv_sec must be equal */

	if (now->tv_usec > control->tv_usec) return 1;						/* Control time has been past by useconds */
	if (now->tv_usec < control->tv_usec) return 0;						/* Control time hasnt been reached yet */
	
	return 0;										/* Dead heat. Who'da thought! */
}

int i_time_diff_seconds (struct timeval *now, struct timeval *then)				/* Return the seconds different between the two timevals */
{
	return (now->tv_sec - then->tv_sec);
}

int i_time_subtract (struct timeval *pass_x, struct timeval *pass_y, struct timeval *result)
{
  /* Subtract Y from X, place the result in result
   * 
   * Returns 0 where result is positive
   * Return 1 where result is negative
   */

  struct timeval x;
  struct timeval y;

  /* Work off local copies of X and Y to ensure this 
   * function doesnt alter the passed structs 
   */
  
  x.tv_sec = pass_x->tv_sec;
  x.tv_usec = pass_x->tv_usec;

  y.tv_sec = pass_y->tv_sec;
  y.tv_usec = pass_y->tv_usec;

  /* Perform the carry for the later subtraction by updating y. */

  if (x.tv_usec < y.tv_usec) 
  {
    int nsec = (y.tv_usec - x.tv_usec) / 1000000 + 1;

    y.tv_usec -= 1000000 * nsec;
    y.tv_sec += nsec;
  }
  
  if (x.tv_usec - y.tv_usec > 1000000) 
  {
    int nsec = (x.tv_usec - y.tv_usec) / 1000000;
    
    y.tv_usec += 1000000 * nsec;
    y.tv_sec -= nsec;
  }

  /* Compute the time remaining to wait. tv_usec is certainly positive. */

  result->tv_sec = x.tv_sec - y.tv_sec;
  result->tv_usec = x.tv_usec - y.tv_usec;

  /* Return 1 if result is negative. */
  return x.tv_sec < y.tv_sec;
}

time_t i_time_weekof (time_t ref_sec, struct tm *tm)
{
  time_t daystart_sec;
  time_t weekstart_sec;
  struct tm daystart_tm;

  /* Calculates the 'week of'
   * i.e the first  before Monday beforethe current time.
   *
   * This is used to keep track of 'week' graphs 
   */

  /* Get the time for the start of the day specified by ref_sec */
  daystart_sec = i_time_daystart (ref_sec, &daystart_tm);

  /* Move to the start of the week (Monday)*/
  if (daystart_tm.tm_wday != 0)
  {
    /* Monday - Saturday */
    weekstart_sec = daystart_sec - ((daystart_tm.tm_wday - 1) * (60 * 60 * 24));
  }
  else
  {
    /* Sunday */
    weekstart_sec = daystart_sec - (6 * (60 * 60 * 24));
  }

  /* Store week_start seconds in passed tm struct */
  localtime_r (&weekstart_sec, tm);

  return weekstart_sec;
}

time_t i_time_monthend (time_t ref_sec, struct tm *tm)
{
  time_t month_end;
  struct tm ref_tm;

  /* Find where ref_sec is */
  
  localtime_r (&ref_sec, &ref_tm);              /* Get the time struct for the ref_sec */

  /* Move to the next month */
  
  if (ref_tm.tm_mon == 11)
  { ref_tm.tm_year++; ref_tm.tm_mon = 0; }      /* Go to next month (first month in the next year) */
  else
  { ref_tm.tm_mon++; }                          /* Go to next month */

  /* Move to the first second of the next month */
  
  ref_tm.tm_mday = 1;                           /* Go to the first day of this month */
  ref_tm.tm_hour = 0;                           /* Go to the first hour of this day */
  ref_tm.tm_min = 0;                            /* Go to the first min of this day */
  ref_tm.tm_sec = 0;                            /* Go to the first sec of this day */

  /* Move back on second */

  month_end = mktime (&ref_tm);                 /* Get the seconds for the first second in the first day of the next month */
  month_end--;                                  /* Go back one second, hence the last second, of the last day of the month */
  localtime_r (&month_end, tm);                 /* Set value of tm pointer passed to this func */

  /* Finished */
  
  return month_end;
}

time_t i_time_monthstart (time_t ref_sec, struct tm *tm)
{
  time_t month_start;
  struct tm ref_tm;

  /* Find where ref_sec is */

  localtime_r (&ref_sec, &ref_tm);              /* Get the time struct for the ref_sec */

  /* Move to the first second of the month */

  ref_tm.tm_mday = 1;                           /* Go to the first day of this month */
  ref_tm.tm_hour = 0;                           /* Go to the first hour of this day */
  ref_tm.tm_min = 0;                            /* Go to the first min of this day */
  ref_tm.tm_sec = 0;                            /* Go to the first sec of this day */

  month_start = mktime (&ref_tm);               /* Get the seconds for the first second in the first day of the next month */
  localtime_r (&month_start, tm);               /* Set value of tm pointer passed to this func */

  /* Finished */

  return month_start;
}

time_t i_time_daystart (time_t now, struct tm *tm)
{
  time_t day_start;
  struct tm nowtm;

  localtime_r (&now, &nowtm);
  nowtm.tm_sec = 0;
  nowtm.tm_min = 0;
  nowtm.tm_hour = 0;

  day_start = mktime (&nowtm);
  localtime_r (&day_start, tm);

  return day_start;
}

time_t i_time_yearstart (time_t now, struct tm *tm)
{
  time_t year_start; 
  struct tm nowtm; 

  localtime_r (&now, &nowtm);
  nowtm.tm_sec = 0;
  nowtm.tm_min = 0;
  nowtm.tm_hour = 0;
  nowtm.tm_mon = 0;
  nowtm.tm_mday = 1;

  year_start = mktime (&nowtm);
  localtime_r (&year_start, tm);

  return year_start;
}

time_t i_time_yearend (time_t now, struct tm *tm)
{
  time_t year_end;
  struct tm nowtm;

  localtime_r (&now, &nowtm);
  nowtm.tm_sec = 0;
  nowtm.tm_min = 0;
  nowtm.tm_hour = 0;
  nowtm.tm_mon = 0;
  nowtm.tm_mday = 1;
  nowtm.tm_year++;

  year_end = mktime (&nowtm);
  year_end--;
  localtime_r (&year_end, tm);

  return year_end;
}

char* i_time_month_string (int tm_mon)
{
  switch (tm_mon)
  {
    case 0: return "January";
    case 1: return "February";
    case 2: return "March";
    case 3: return "April";
    case 4: return "May";
    case 5: return "June";
    case 6: return "July";
    case 7: return "August";
    case 8: return "September";
    case 9: return "October";
    case 10: return "November";
    case 11: return "December";
  }

  return "Unknown";
}

char* i_time_interval_str (struct timeval *ref_tv)
{
  /* Returns a string suitable to display the interval
   * given in seconds. E.g 00:00:13, 1d4h45m, 2w3d5h
   */

  int days;
  int hours;
  int minutes;
  int seconds;
  int centiseconds;
  time_t ref_sec;
  time_t ref_usec;
  char *str;

  ref_sec = ref_tv->tv_sec;
  ref_usec = ref_tv->tv_usec;
  
  days = ref_sec / (60 * 60 * 24);
  ref_sec %= (60 * 60 * 24);
  
  hours = ref_sec / (60 * 60);
  ref_sec %= (60 * 60);

  minutes = ref_sec / 60;
  seconds = ref_sec % 60;

  centiseconds = ref_usec / 10000;

  if (days < 1)
  {
    asprintf (&str, "%ih %im %is", hours, minutes, seconds);
  }
  else
  {
    asprintf (&str, "%id %ih %im", days, hours, minutes);
  }

  return str;
}

char* i_time_ctime (time_t ref_sec)
{
  int rtn_strlen;
  char *ctime_str;
  char *rtn_str;

  ctime_str = ctime (&ref_sec);
  if (!ctime_str) return NULL;

  rtn_str = strdup (ctime_str);
  rtn_strlen = strlen (rtn_str);
  rtn_str[rtn_strlen-1] = '\0';   /* Overwrite the \n */

  return rtn_str;
}
