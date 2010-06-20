#include <stdlib.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "hierarchy.h"
#include "cement.h"
#include "callback.h"
#include "entity.h"

/** \addtogroup entity_refresh Entity Refresh
 * @ingroup entity
 * @{
 */

extern i_resource *global_self;

/* 
 * Entity Refresh Callback Functions 
 */

/* Add */

i_callback* i_entity_refreshcb_add (i_entity *ent, int (*cbfunc) (), void *passdata)
{
  int num;
  i_callback *cb;

  /* Create callback */
  cb = i_callback_create ();
  if (!cb)
  { i_printf (1, "i_entity_refreshcb_add failed to create callback struct"); return NULL; }
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Check/Create refreshcb_list */
  if (!ent->refreshcb_list)
  {
    /* Create refreshcb_list */
    ent->refreshcb_list = i_list_create ();
    if (!ent->refreshcb_list)
    { i_printf (1, "i_entity_refreshcb_add failed to create ent->refreshcb_list"); i_entity_free (ent); return NULL; }
  }

  /* Enqueue */ 
  num = i_list_enqueue (ent->refreshcb_list, cb);
  if (num != 0)
  { 
    i_printf (1, "i_entity_refreshcb_add failed to enqueue callback for %s %s",
      i_entity_typestr (ent->ent_type), ent->name_str);
    i_callback_free (cb);
    return NULL;
  }

  return cb;
}

/* Remove */

int i_entity_refreshcb_remove (i_callback *cb)
{
  /* Just NULLify the cb, it will be
   * removed by the execlist func 
   */

  cb->func = NULL;
  cb->passdata = NULL;

  return 0;
}

/* Exec */

int i_entity_refreshcb_execlist (i_resource *self, i_entity *ent)
{
  i_callback *refcb;

  /* Execute the callbacks and perform
   * callback list maint where necessary
   */
  for (i_list_move_head(ent->refreshcb_list); (refcb=i_list_restore(ent->refreshcb_list))!=NULL; i_list_move_next(ent->refreshcb_list))
  {
    int num;
    int remove = 0;
    if (refcb->func)
    {
      num = refcb->func (self, ent, refcb->passdata);
      if (num != 0) remove = 1;
    }
    else
    { remove = 1; }
    if (remove == 1)
    { 
      num = i_list_search (ent->refreshcb_list, refcb);
      if (num == 0)
      {
        i_list_delete (ent->refreshcb_list); 
        i_callback_free (refcb);
      }
    }
  }

  return 0;
}

/* @} */
