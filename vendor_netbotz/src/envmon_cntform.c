#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/ip.h>
#include <induction/timeutil.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/interface.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>
#include <lithium/snmp.h>

#include "envmon.h"

int v_envmon_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_container *cnt = (i_container *) ent;
  i_form_item *fitem = NULL;

  /* 
   * Environmental Monitoring Container Form
   */

  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);
  
  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *str;
    char *fsroot;
    i_metric *met;
    v_envmon_item *env;
    i_object *master_obj;
    
    master_obj = v_envmon_masterobj ();
    if (!master_obj) return -1;
    env = (v_envmon_item *) master_obj->itemptr;

    for (i_list_move_head(master_obj->met_list); (met=i_list_restore(master_obj->met_list))!=NULL; i_list_move_next(master_obj->met_list))
    {
      str = i_metric_valstr (met, NULL);
      fitem = i_form_string_add (reqdata->form_out, met->name_str, met->desc_str, str);
      i_form_item_add_link (fitem, 0, 0, 0, RES_ADDR(self), ENT_ADDR(met), NULL, 0, NULL, 0);
      free (str);
    }

    fsroot = i_entity_path (self, ENTITY(env->campic_small), 0, ENTPATH_ROOT);
    asprintf (&str, "%s/campic.jpg", fsroot);
    fitem = i_form_image_add (reqdata->form_out, "picture", "Image from camera", NULL, str);
    i_form_item_add_link (fitem, 0, 0, 0, RES_ADDR(self), ENT_ADDR(env->campic_large), NULL, 0, NULL, 0);
    free (fsroot);
    free (str);
    
    i_form_string_add (reqdata->form_out, "envcond_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { fitem = i_form_string_add (reqdata->form_out, "error", "Condition list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { fitem = i_form_string_add (reqdata->form_out, "error", "Condition list population is in progress", NULL); }
    else
    { fitem = i_form_string_add (reqdata->form_out, "error", "Condition list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
