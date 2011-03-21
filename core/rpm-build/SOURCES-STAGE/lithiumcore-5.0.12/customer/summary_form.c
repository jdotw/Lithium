#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/postgresql.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/hashtable.h>
#include <induction/entity.h>
#include <induction/cement.h>
#include <induction/colour.h>

#include "infstat.h"
#include "case.h"
#include "incident.h"

int form_summary (i_resource *self, i_form_reqdata *reqdata) 
{ 
  int row;
  char *labels[2]; 
  i_form_item *item;
  
  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_summary_form failed to create form"); return -1; }

  /*
   * Entities
   */

  i_form_frame_start (reqdata->form_out, "incidents_frame", "Entity States");
  item = i_form_table_create (reqdata->form_out, "incidents_status", NULL, 2);
  if (!item) { i_printf (1, "form_summary_form failed to create table"); return -1; }
//  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_NOHEADERS, 0, 0, 0, NULL, 0);            /* No header row */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
  i_form_item_add_option (item, ITEM_OPTION_ALIGN, 1, 1, 0, "RIGHT", 6);
  i_form_item_add_option (item, ITEM_OPTION_ALIGN, 1, 1, 1, "RIGHT", 6);
  i_form_item_add_option (item, ITEM_OPTION_ALIGN, 1, 1, 2, "RIGHT", 6);

  labels[0] = "Failed";
  asprintf (&labels[1], "%lu", l_incident_count_failed());
  row = i_form_table_add_row (item, labels);
  free (labels[1]);
  if (l_incident_count_failed() > 0)
  { 
    i_form_item_add_option (item, ITEM_OPTION_FGCOLOR, 1, 0, row, i_colour_fg_str(ENTSTATE_FAILED), 8); 
    i_form_item_add_option (item, ITEM_OPTION_FGCOLOR, 1, 1, row, i_colour_fg_str(ENTSTATE_FAILED), 8); 
  }

  labels[0] = "Impaired";
  asprintf (&labels[1], "%lu", l_incident_count_impaired());
  row = i_form_table_add_row (item, labels);
  free (labels[1]);
  if (l_incident_count_impaired() > 0)
  { 
    i_form_item_add_option (item, ITEM_OPTION_FGCOLOR, 1, 0, row, i_colour_fg_str(ENTSTATE_IMPAIRED), 8); 
    i_form_item_add_option (item, ITEM_OPTION_FGCOLOR, 1, 1, row, i_colour_fg_str(ENTSTATE_IMPAIRED), 8); 
  }

  labels[0] = "At Risk";
  asprintf (&labels[1], "%lu", l_incident_count_atrisk());
  row = i_form_table_add_row (item, labels);
  free (labels[1]);
  if (l_incident_count_atrisk() > 0)
  { 
    i_form_item_add_option (item, ITEM_OPTION_FGCOLOR, 1, 0, row, i_colour_fg_str(ENTSTATE_ATRISK), 8); 
    i_form_item_add_option (item, ITEM_OPTION_FGCOLOR, 1, 1, row, i_colour_fg_str(ENTSTATE_ATRISK), 8); 
  }

  i_form_frame_end (reqdata->form_out, "incidents_frame");

  return 1;
}

