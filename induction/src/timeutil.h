#include <time.h>

int i_time_check_past (struct timeval *now, struct timeval *control);				/* Is now past then... if so, then a 1 is returned */
int i_time_diff_seconds (struct timeval *now, struct timeval *then);
int i_time_subtract (struct timeval *pass_x, struct timeval *pass_y, struct timeval *result);
time_t i_time_weekof (time_t now, struct tm *tm);
time_t i_time_monthend (time_t now, struct tm *tm);
time_t i_time_monthstart (time_t ref_sec, struct tm *tm);
time_t i_time_daystart (time_t now, struct tm *tm);
time_t i_time_yearstart (time_t now, struct tm *tm);
time_t i_time_yearend (time_t now, struct tm *tm);
char* i_time_month_string (int tm_mon);
char* i_time_interval_str (struct timeval *ref_tv);
char* i_time_ctime (time_t ref_sec);
