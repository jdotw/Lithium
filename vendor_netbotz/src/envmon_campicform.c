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

int v_envmon_campicform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  char *fsroot;
  v_envmon_item *env;
  i_object *master_obj;
  i_form_item *fitem = NULL;
  i_metric *met = (i_metric *) ent;

  /* 
   * Environmental Monitoring Camera picture form
   */

  i_form_frame_start (reqdata->form_out, met->name_str, met->desc_str);
  
  master_obj = v_envmon_masterobj ();
  if (!master_obj) return -1;
  env = (v_envmon_item *) master_obj->itemptr;

  str = i_metric_valstr (met, NULL);
  fitem = i_form_string_add (reqdata->form_out, met->name_str, met->desc_str, str);
  i_form_item_add_link (fitem, 0, 0, 0, RES_ADDR(self), ENT_ADDR(met), NULL, 0, NULL, 0);
  free (str);
                  
  fsroot = i_entity_path (self, ENTITY(met), 0, ENTPATH_ROOT);
  asprintf (&str, "%s/campic.jpg", fsroot);
  fitem = i_form_image_add (reqdata->form_out, "picture", "Image from camera", NULL, str);
  free (fsroot);
  free (str);

  i_form_string_add (reqdata->form_out, "envcond_note", "Note", "'*' next to a value indicates the data is not current");

  i_form_frame_end (reqdata->form_out, met->name_str);

  return 1;
}
