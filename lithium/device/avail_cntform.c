#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/ip.h>
#include <induction/timeutil.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/form.h>
#include <induction/str.h>

#include "avail.h"

int l_avail_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_container *cnt;
  l_avail_item *avail;
  i_form_item *item;
  char *labels[3] = { "Description", "Availability", "Response Time" };

  /*
   * Avail object table 
   */

  i_form_frame_start (reqdata->form_out, "avail_objlist", "Availability Objects");

  cnt = l_avail_cnt ();

  item = i_form_table_create (reqdata->form_out, "availobjlist", NULL, 3);
  if (!item) { i_printf (1, "l_avail_cntform failed to create table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (item, labels);

  for (i_list_move_head(cnt->item_list); (avail=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
  {
    int row;

    labels[0] = avail->obj->desc_str;
    labels[1] = i_metric_valstr (avail->ok_pc, NULL);
    labels[2] = i_metric_valstr (avail->resptime, NULL);

    row = i_form_table_add_row (item, labels);

    i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(avail->obj), NULL, 0, NULL, 0);

    free (labels[1]);
    free (labels[2]);
  }

  i_form_string_add (reqdata->form_out, "availlist_note", "Note", "'*' next to a value indicates the data is not current");

  i_form_frame_end (reqdata->form_out, "avail_objlist");

  return 1;
}

int l_avail_cntform_summary (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_form_item *item;
  i_object *masterobj;
  l_avail_item *masteritem;

  masterobj = l_avail_masterobj ();
  if (!masterobj) return 1;
  masteritem = masterobj->itemptr;

  i_form_frame_start (reqdata->form_out, "avail_summary", "Availability");
  
  /* Availability graph */
  item = i_form_metcgraph_add (self, reqdata->form_out, masteritem->avail_cg, GRAPHSIZE_SMALL, 0, GRAPHPERIOD_LAST48HR);

  i_form_frame_end (reqdata->form_out, "avail_summary");

  return 1;
}

int l_avail_cntform_rtsummary (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_form_item *item;
  i_object *masterobj;
  l_avail_item *masteritem;

  masterobj = l_avail_masterobj ();
  if (!masterobj) return 1;
  masteritem = masterobj->itemptr;

  i_form_frame_start (reqdata->form_out, "avail_rt", "Response Time");

  /* Response time graph */
  item = i_form_metgraph_add (self, reqdata->form_out, masteritem->resptime, GRAPHSIZE_SMALL, 0, GRAPHPERIOD_LAST48HR);

  i_form_frame_end (reqdata->form_out, "avail_rtsummary");

  return 1;
}


