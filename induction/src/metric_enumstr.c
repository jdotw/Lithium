#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "induction.h"
#include "list.h"
#include "ip.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"

/** \addtogroup metric Metrics
 * @{
 */

/* 
 * Metric Enumerated Strings
 */

/* Struct Manipulation */

i_metric_enumstr* i_metric_enumstr_create ()
{
  i_metric_enumstr *estr;

  estr = (i_metric_enumstr *) malloc (sizeof(i_metric_enumstr));
  if (!estr)
  { i_printf (1, "i_metric_enumstr_create failed to malloc i_metric_enumstr struct"); return NULL; }
  memset (estr, 0, sizeof(i_metric_enumstr));

  return estr;
}

void i_metric_enumstr_free (void *estrptr)
{
  i_metric_enumstr *estr = estrptr; 

  if (!estr) return;

  if (estr->str) free (estr->str);

  free (estr);
}

/* Add */

int i_metric_enumstr_add (i_metric *met, int val_int, char *str)
{
  int num;
  i_metric_enumstr *estr;

  if (!met) return -1;

  /* Check/Create List */
  if (!met->enumstr_list)
  {
    met->enumstr_list = i_list_create ();
    if (!met->enumstr_list)
    { i_printf (1, "i_metric_enumstr_add failed to create met->enumstr_list"); return -1; }
    i_list_set_destructor (met->enumstr_list, i_metric_enumstr_free);
  }

  /* Create estr */
  estr = i_metric_enumstr_create ();
  if (!estr)
  { i_printf (1, "i_metric_enumstr_add failed to create estr struct"); return -1; }
  estr->val_int = val_int;
  if (str)
  { estr->str = strdup (str); }

  /* Enqueue */
  num = i_list_enqueue (met->enumstr_list, estr);
  if (num != 0)
  { i_printf (1, "i_metric_enumstr_add failed to enqueue estr"); i_metric_enumstr_free (estr); return -1; }

  return 0;
}

/* Evaluate */

char* i_metric_enumstr_eval (i_metric *met, int val_int)
{
  /* Iterates through the met->enumstr_list and returns
   * the first match for the value of 'val'
   */

  i_metric_enumstr *estr;
  
  for (i_list_move_head(met->enumstr_list); (estr=i_list_restore(met->enumstr_list))!=NULL; i_list_move_next(met->enumstr_list))
  { if (estr->val_int == val_int) return estr->str; }

  return NULL;
}

/* @} */
