#include <stdlib.h>

#include "induction.h"
#include "postgresql.h"
#include "timeutil.h"
#include "list.h"

static float static_op_avgresptime_ms = 0;
static float static_query_avgresptime_ms = 0;

int i_pg_async_resptime_record (i_resource *self, i_pg_async_conn *conn)
{
  int num;
  struct timeval diff_tv;
  struct timeval now;
  float resptime_ms = 0;

  if (!conn || !conn->query) return -1;

  gettimeofday (&now, NULL);

  /* Operation Response Time */

  num = i_time_subtract (&now, &conn->query->last_result, &diff_tv);
  if (num != 0)
  { i_printf (1, "i_pg_async_resptime_record failed, start is after end. clock skew?"); return -1; }
  resptime_ms = diff_tv.tv_sec * 1000;      /* Multiply the seconds */
  resptime_ms += diff_tv.tv_usec / 1000;    /* divide the microseconds */
  static_op_avgresptime_ms = static_op_avgresptime_ms + resptime_ms;
  static_op_avgresptime_ms = static_op_avgresptime_ms / 2;

  /* Query Response Time */

  if (conn->query->operations && conn->query->operations->size < 1)
  {
    /* End of query, check response time */
    num = i_time_subtract (&now, &conn->query->start, &diff_tv);
    if (num != 0)
    { i_printf (1, "i_pg_async_resptime_record failed, start is after end. clock skew?"); return -1; }
    resptime_ms = diff_tv.tv_sec * 1000;      /* Multiply the seconds */
    resptime_ms += diff_tv.tv_usec / 1000;    /* divide the microseconds */
    static_query_avgresptime_ms = static_query_avgresptime_ms + resptime_ms;
    static_query_avgresptime_ms = static_query_avgresptime_ms / 2;
  }    

  return 0;
}
