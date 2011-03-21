#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <induction.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/hashtable.h>
#include <induction/respond.h>
#include <induction/auth.h>
#include <induction/form.h>

#include "main.h"
#include "reqtime.h"

static l_reqtime *static_reqtime;

void l_reqtime_start ()
{
  if (static_reqtime) free (static_reqtime);

  static_reqtime = (l_reqtime *) malloc (sizeof(l_reqtime));
  memset (static_reqtime, 0, sizeof(static_reqtime));

  gettimeofday (&static_reqtime->start, NULL);
}

void l_reqtime_registered ()
{
  gettimeofday (&static_reqtime->registered, NULL);
}

void l_reqtime_reqrecvd ()
{
  if (!static_reqtime)
  { 
    l_reqtime_start ();
  }

  gettimeofday (&static_reqtime->reqrecvd, NULL);
}

void l_reqtime_authsent ()
{
  gettimeofday (&static_reqtime->authsent, NULL);
}

void l_reqtime_authrecvd ()
{
  gettimeofday (&static_reqtime->authrecvd, NULL);
}

void l_reqtime_reqrelayed ()
{
  gettimeofday (&static_reqtime->reqrelayed, NULL);
}

void l_reqtime_resprecvd ()
{
  time_t reg_interval;
  time_t auth_interval;
  time_t req_interval;
  time_t total_interval;

  gettimeofday (&static_reqtime->resprecvd, NULL);

  reg_interval = static_reqtime->registered.tv_sec - static_reqtime->start.tv_sec;
  auth_interval = static_reqtime->authrecvd.tv_sec - static_reqtime->authsent.tv_sec;
  req_interval = static_reqtime->resprecvd.tv_sec - static_reqtime->reqrelayed.tv_sec;
  total_interval = static_reqtime->resprecvd.tv_sec - static_reqtime->start.tv_sec;

  i_printf (0, "l_reqtime_resprecvd reg_interval=%lis auth_interval=%lis req_interval=%lis total_interval=%lis",
    reg_interval, auth_interval, req_interval, total_interval);
  
  free (static_reqtime);
  static_reqtime = NULL;
}

