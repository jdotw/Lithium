#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include "induction.h"
#include "path.h"
#include "form.h"
#include "navtree.h"
#include "navform.h"
#include "cement.h"
#include "entity.h"
#include "construct.h"
#include "hierarchy.h"
#include "customer.h"
#include "site.h"
#include "device.h"
#include "auth.h"
#include "data.h"
#include "list.h"
#include "log.h"

/*
 * Logging system for capturing mainly i_printfs
 */

static int static_enabled = 0;
extern i_resource *global_self;
static FILE *static_log_file = NULL;

/* Enable/Disable */

int i_log_enable (i_resource *self, char *log_filename)
{
  if (static_enabled == 1)
  { i_printf (1, "i_log_enable warning, sub-system already enabled"); return -1; }

  static_enabled = 1;

  char *filename = NULL;
  switch (self->type)
  {
    case RES_CUSTOMER:
      asprintf (&filename, "customer_%s.log", self->ident_str);
      break;
    case RES_DEVICE:
      asprintf (&filename, "device_%s_%s_%s.log", self->hierarchy->cust->name_str, self->hierarchy->site->desc_esc_str, self->hierarchy->dev->desc_esc_str);
      break;
    case RES_ADMIN:
      asprintf (&filename, "admin.log");
      break;
    case RES_CORE:
      asprintf (&filename, "lithium.log");
      break;
    case RES_CLIENT_HANDLER_CORE:
      asprintf (&filename, "client_handler.log");
      break;
    case RES_CLIENT_HANDLER_CHILD:
      asprintf (&filename, "client.log");
      break;
    default:
      asprintf (&filename, "misc.log");
      break;
  }

  if (filename)
  {
    char *fullpath = i_path_glue (self->construct->log_path, filename);
    static_log_file = fopen (fullpath, "a");
    if (!static_log_file)
    { i_printf (0, "i_log_enable failed to open log file %s", fullpath); }
    free (fullpath);
    free (filename);
  }
  else
  { i_printf (0, "i_log_enable no log-file specified for process"); }

  return 0;
}

int i_log_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "i_log_enable warning, sub-system already disabled"); return -1; }

  static_enabled = 0;

  if (static_log_file)
  {
    fclose (static_log_file);
    static_log_file = NULL;
  }

  return 0;
}

int i_log_init (i_resource *self)
{
  if (static_enabled == 1)
  { i_log_disable (self); }

  return 0;
}

/* Log */

int i_log_string (int level, char *string)
{
  if (static_enabled == 0) return 0;

  if (static_log_file) 
  {
    fprintf (static_log_file, "%s\n", string);
    fflush (static_log_file);
  }

  return 0;
}

