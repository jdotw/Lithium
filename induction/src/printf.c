#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdarg.h>
#include <time.h>
#include <syslog.h>
#include <dlfcn.h>

#include "induction.h"
#include "construct.h"
#include "log.h"

static void (*printf_wrapper_func) (int level, char *format, va_list ap) = NULL;

void i_debug (char *format, ...)
{
#ifdef DEBUG
  va_list ap;
  va_start (ap, format);
  i_vaprintf (0, format, ap);
  va_end (ap);
#endif
}

void i_printf (int level, char *format, ...)						/* Level 0 = Always Logged, 1 = Higher level debug, 2 = Crazy */
{
  va_list ap;
  va_start (ap, format);
  i_vaprintf (level, format, ap);
  va_end (ap);
}

void i_vaprintf (int level, char *format, va_list ap)						/* Level 0 = Always Logged, 1 = Higher level debug, 2 = Crazy */
{
  int pid;
  time_t timet;
  struct tm *tms;
  char *time_str;
  char *type_str;
  char *ap_str;
  char *print_str;
  i_resource *self;

  self = i_resource_self ();
  if (self)
  {
    if (level > self->log_level) return;  /* Ignore this message as it is below our level */
  }
  else 
  {
    /* No self variable, hence this is probably a client. Try and use
     * i_printf_wrapper
     */
    if (printf_wrapper_func) printf_wrapper_func (level, format, ap);
    return; 
  }

  pid = getpid ();

  /* Prepare time string */
	
  timet = time (NULL);
  tms = localtime(&timet);
  asprintf (&time_str, "%.2i:%.2i:%.2i %.2i%.2i%.2i ", tms->tm_hour, tms->tm_min, tms->tm_sec, tms->tm_year-100, tms->tm_mon+1, tms->tm_mday);

  /* Prepare resource type string */

  type_str = i_resource_info_type_string (self);
  if (!type_str)
  { type_str = strdup ("Unknown"); }

  /* Prepare string from variable args */

  vasprintf (&ap_str, format, ap);

  /* Format message string */

  if (self) asprintf (&print_str, "[%i] %s [%i] [%s:%s:%i:%i:%s] (%s) - %s", level, time_str, getpid(), self->plexus, self->node, self->type, self->ident_int, self->ident_str, type_str, ap_str);
  else asprintf (&print_str, "[%i] %s [%i] (%s) - %s", level, time_str, getpid(), type_str, ap_str);
  
  free (time_str);
  free (ap_str);
  free (type_str);

  /* Log */
  i_log_string (level, print_str);

  /* Print */
  if (self->construct && self->construct->quiet == 0)
  { fprintf (stdout, "%s\n", print_str); }

  free (print_str);
}

void i_printf_set_wrapper (void (*func) ())
{
  printf_wrapper_func = func;
}
