#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

#include <induction.h>

void c_segfault_handler (int num)
{
  i_printf (1, "## SEGFAULT ##");
  syslog (LOG_EMERG, "lithium pid %i segfaulted", getpid());
  exit (2);
}

void c_abort_handler (int num)
{
  i_printf (1, "## ABORT ##");
//  syslog (LOG_INFO, "lithium pid %i aborted -- paused for debug", getpid());
//  while (1)
//  {
//    sleep (2);
//  }
  exit (1);
}

void c_sigchld_handler (int signo)
{
  int status;

  /* Wait for any child without blocking */
  waitpid(-1, &status, WNOHANG);
}

void c_sighup_handler (int signo)
{ exit (0); }
