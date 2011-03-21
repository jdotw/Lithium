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

#include "nwtput.h"

int l_nwtput_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  int row;
  i_container *cnt;
  i_object *bpsobj;
  i_object *ppsobj;
  l_nwtput_item *bpsitem;
  l_nwtput_item *ppsitem;
  i_form_item *item;
  char *labels[3] = { "Metric", "Input", "Output" };

  /*
   * Avail object table 
   */

  cnt = l_nwtput_cnt ();
  bpsobj = l_nwtput_bpsobj ();
  ppsobj = l_nwtput_ppsobj ();
  if (!cnt || !bpsobj || !ppsobj) return -1;

  bpsitem = (l_nwtput_item *) bpsobj->itemptr;
  ppsitem = (l_nwtput_item *) ppsobj->itemptr;

  i_form_frame_start (reqdata->form_out, "nwtput", "Aggregate Network Throughput");

  item = i_form_table_create (reqdata->form_out, "nwtputobjlist", NULL, 3);
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 1, 0, 0, "100", 4);
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 1, 1, 0, "80", 3);              
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 1, 2, 0, "80", 3);              
  i_form_table_add_row (item, labels);

  /* Packets/sec */
  labels[0] = "Packets per Second";
  labels[1] = i_metric_valstr (ppsitem->input, NULL);
  labels[2] = i_metric_valstr (ppsitem->output, NULL);
  row = i_form_table_add_row (item, labels);
  i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(ppsobj), NULL, 0, NULL, 0);
  if (labels[1]) free (labels[1]);
  if (labels[2]) free (labels[2]);

  /* Bits/sec */
  labels[0] = "Bits per Second";
  labels[1] = i_metric_valstr (bpsitem->input, NULL);
  labels[2] = i_metric_valstr (bpsitem->output, NULL);
  row = i_form_table_add_row (item, labels);
  i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(bpsobj), NULL, 0, NULL, 0);
  if (labels[1]) free (labels[1]);
  if (labels[2]) free (labels[2]);

  /* End frame */
  i_form_string_add (reqdata->form_out, "nwtputlist_note", "Note", "'*' next to a value indicates the data is not current");
  i_form_frame_end (reqdata->form_out, "nwtput_objlist");

  return 1;
}

