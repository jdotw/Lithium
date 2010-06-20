#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "path.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"

/** \addtogroup metric_record Recording and Graphing
 * @ingroup metric
 * @{
 */

/* 
 * Metric Recording (RRD and SQL)
 */

int i_metric_record (i_resource *self, i_metric *met)
{
  /* Perform the configured recording procedure 
   * for the given metric.
   */

  int num;
//  int valfd;
//  int valrawfd;
//  char *fsroot;
//  char *val_fullpath;
//  char *valraw_fullpath;
//  i_metric_value *val;

  /* val/val_raw File Recording 
   *
   * The list of values are written to files
   * 'val' and 'val_raw' in the entity root path/
   */

  /* Open Files */
  /* FIX HACK Castnet
  fsroot = i_entity_path (self, ENTITY(met), 0, ENTPATH_ROOT);
  val_fullpath = i_path_glue (fsroot, "val");
  if (fsroot && val_fullpath)
  { valfd = open (val_fullpath, O_WRONLY|O_CREAT|O_TRUNC, 0750); }
  else
  { valfd = -1; }
  if (val_fullpath) free (val_fullpath);
  valraw_fullpath = i_path_glue (fsroot, "val_raw");
  if (fsroot && valraw_fullpath)
  { valrawfd = open (valraw_fullpath, O_WRONLY|O_CREAT|O_TRUNC, 0750); }
  else
  { valrawfd = -1; }
  if (valraw_fullpath) free (valraw_fullpath);
  free (fsroot);
  */

  /* Write values */
  /* FIX HACK Castnet
  for (i_list_move_head(met->val_list); (val=i_list_restore(met->val_list))!=NULL; i_list_move_next(met->val_list))
  {
    char *timestr;
    char *valstr;
    char *writestr;

    timestr = ctime((time_t *)&val->tstamp.tv_sec);
    */
    /* write to val file */
    /* FIX HACK Castnet
    valstr = i_metric_valstr (met, val);
    if (valstr && valfd != -1)
    {
    write (valfd, timestr, strlen(timestr)-1);    *//* -1 to remove the \n from ctime */  
  
  /* FIX HACK Castnet
      asprintf (&writestr, " - %s\n", valstr);
      write (valfd, writestr, strlen(writestr));
      free (writestr);
    }
    if (valstr) free (valstr);

    */
    
    /* write to val_raw file */
  /* FIX HACK Castnet
    valstr = i_metric_valstr_raw (met, val);
    if (valstr && valrawfd != -1) 
    {
      asprintf (&writestr, "%li %s\n", val->tstamp.tv_usec, valstr); 
      write (valrawfd, writestr, strlen(writestr));
      free (writestr);
    }
    if (valstr) free (valstr);
  }
  if (valfd != -1) close (valfd);
  if (valrawfd != -1) close (valrawfd);

  */
  /* Handle RRD Recording 
   *
   * If the RECMETHOD_RRD flag is set, the 
   * i_metric_record_rrd func is called
   */
  if (met->record_method & RECMETHOD_RRD && met->record_enabled == 1)
  {
    num = i_metric_record_rrd (self, met);
    if (num != 0)
    { 
      i_printf (1, "i_metric_record warning, failed to perform RRD recording for %s %s", 
        i_entity_typestr (met->ent_type), met->name_str);
    }
  }

  /* Handle SQL Recording
   *
   * If the RECMETHOD_SQL flag is set, the
   * i_metric_record_sql func is called
   */
  if (met->record_method & RECMETHOD_SQL && met->record_enabled == 1)
  {
    num = i_metric_record_sql (self, met);
    if (num != 0)
    { 
      i_printf (1, "i_metric_record warning, failed to perform SQL recording for %s %s",
        i_entity_typestr (met->ent_type), met->name_str);
    }
  }

  return 0;
}
