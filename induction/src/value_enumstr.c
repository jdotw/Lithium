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
#include "value.h"

/* 
 * Value Enumerated Strings
 */

/* Struct Manipulation */

i_value_enumstr* i_value_enumstr_create ()
{
  i_value_enumstr *estr;

  estr = (i_value_enumstr *) malloc (sizeof(i_value_enumstr));
  if (!estr)
  { i_printf (1, "i_value_enumstr_create failed to malloc i_value_enumstr struct"); return NULL; }
  memset (estr, 0, sizeof(i_value_enumstr));

  return estr;
}

void i_value_enumstr_free (void *estrptr)
{
  i_value_enumstr *estr = estrptr; 

  if (!estr) return;

  if (estr->str) free (estr->str);

  free (estr);
}

/* Add */

int i_value_enumstr_add (i_list *enumstr_list, long val_int, char *str)
{
  int num;
  i_value_enumstr *estr;

  /* Create estr */
  estr = i_value_enumstr_create ();
  if (!estr)
  { i_printf (1, "i_value_enumstr_add failed to create estr struct"); return -1; }
  estr->val_int = val_int;
  if (str)
  { estr->str = strdup (str); }

  /* Enqueue */
  num = i_list_enqueue (enumstr_list, estr);
  if (num != 0)
  { i_printf (1, "i_value_enumstr_add failed to enqueue estr"); i_value_enumstr_free (estr); return -1; }

  return 0;
}

/* Evaluate */

char* i_value_enumstr_eval (i_list *enumstr_list, int val_int)
{
  /* Iterates through the met->enumstr_list and returns
   * the first match for the value of 'val'
   */

  i_value_enumstr *estr;
  
  for (i_list_move_head(enumstr_list); (estr=i_list_restore(enumstr_list))!=NULL; i_list_move_next(enumstr_list))
  { if (estr->val_int == val_int) return estr->str; }

  return NULL;
}
