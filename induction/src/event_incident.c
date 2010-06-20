#include <stdlib.h>

#include "induction.h"
#include "event.h"
#include "hierarchy.h"
#include "list.h"
#include "threshold.h"

/* Struct manipulation */

i_event_inc* i_event_inc_create ()
{
  i_event_inc *inc;

  inc = (i_event_inc *) malloc (sizeof(i_event_inc));
  if (!inc)
  { i_printf (1, "i_event_inc_create failed to malloc inc struct"); return NULL;  }
  memset (inc, 0, sizeof(i_event_inc));

  inc->id = -1;  /* Not yet allocated */

  inc->occ_list = i_list_create ();
  if (!inc->occ_list)
  {
    i_printf (1, "i_event_inc_create failed to create occurences list");
    i_event_inc_free (inc);
    return NULL; 
  }
  i_list_set_destructor (inc->occ_list, i_event_inc_occ_free);

  return inc;
}

void i_event_inc_free (void *incptr)
{
  i_event_inc *inc = incptr;

  if (!inc) return;

  if (inc->object_data) free (inc->object_data);
  if (inc->thold) i_threshold_free (inc->thold);
  if (inc->hierarchy) i_hierarchy_free (inc->hierarchy);
  if (inc->occ_list) i_list_free (inc->occ_list);

  free (inc);
}

/* Incident occurrence struct manipulation */

i_event_inc_occ* i_event_inc_occ_create ()
{
  i_event_inc_occ *occ;

  occ = (i_event_inc_occ *) malloc (sizeof(i_event_inc_occ));
  if (!occ) 
  { i_printf (1, "i_event_inc_occ_create failed to malloc occ"); return NULL; }
  memset (occ, 0, sizeof(i_event_inc_occ));

  occ->id = -1; /* Not yet allocated */
  
  return occ;
}

void i_event_inc_occ_free (void *occptr)
{
  i_event_inc_occ *occ = occptr;

  if (!occ) return;

  if (occ->hline_str) free (occ->hline_str);
  if (occ->desc_str) free (occ->desc_str);

  free (occ);
}
