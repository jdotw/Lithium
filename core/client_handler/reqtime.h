typedef struct l_reqtime_s
{
  struct timeval start;
  struct timeval registered;
  struct timeval reqrecvd;
  struct timeval authsent;
  struct timeval authrecvd;
  struct timeval reqrelayed;
  struct timeval resprecvd;
} l_reqtime;

void l_reqtime_start ();
void l_reqtime_registered ();
void l_reqtime_reqrecvd ();
void l_reqtime_authsent ();
void l_reqtime_authrecvd ();
void l_reqtime_reqrelayed ();
void l_reqtime_resprecvd ();
