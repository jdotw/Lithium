#include <stdlib.h>

#include <induction.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/inventory.h>
#include <induction/construct.h>
#include <induction/hierarchy.h>
#include <induction/vendor.h>

#include "avail.h"
#include "devform.h"

/* Device main form */

int l_devform_generate (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_container *cnt;

  /*
   * FIX Status Info should go here 
   */

  /*
   * Per-container frames 
   */
  for (i_list_move_head(ent->child_list); (cnt=i_list_restore(ent->child_list))!=NULL; i_list_move_next(ent->child_list))
  {
    if (cnt->sumform_func)
    { cnt->sumform_func (self, cnt, reqdata); }
  }

  /*
   * Response time frame
   * A separate call is made for response time
   * to have it displayed at the bottom of the devform
   */

  cnt = l_avail_cnt ();
  if (cnt)
  { l_avail_cntform_rtsummary (self, ENTITY(cnt), reqdata); }
  
  return 1;
}
