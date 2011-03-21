#include <stdlib.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/timeutil.h>
#include <induction/list.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/colour.h>
#include <induction/auth.h>
#include <induction/data.h>
#include <induction/incident.h>

#include "case.h"
#include "incident.h"

int form_incident_list (i_resource *self, i_form_reqdata *reqdata)
{
  i_incident *inc;
  i_list *inc_list;

  /* 
   * Form Setup
   */
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT) 
  { return i_form_deliver_denied(self, reqdata); }
  
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out) 
  { i_printf (1, "form_incident_list failed to create form"); return -1; }

  /*
   * Render Handled Incident List
   */

  i_form_frame_start (reqdata->form_out, "uhincidentlist_frame", "Unhandled Incidents");

  /* Iterate incident list */
  inc_list = l_incident_uhlist ();
  for (i_list_move_head(inc_list); (inc=i_list_restore(inc_list))!=NULL; i_list_move_next(inc_list))
  {
    int row;
    char *id_str;
    char *inc_str;
    char *headline;
    char *labels[3] = { "", "", "" };
    i_form_item *fstr;
    i_form_item *table;

    /* Add headline string */
    asprintf (&inc_str, "%s Incident", i_incident_typestr (inc->type));
    asprintf (&id_str, "%li_headline", inc->id);
    asprintf (&headline, "%s at %s", inc->ent->dev_desc, inc->ent->site_desc);
    fstr = i_form_string_add (reqdata->form_out, id_str, inc_str, headline);
    free (inc_str);
    free (id_str);
    free (headline);
    i_form_string_set_fgcolor (fstr, i_colour_fg_str(inc->ent->opstate), i_colour_fg_str(inc->ent->opstate));

    /* Create table */
    asprintf (&id_str, "%li_table", inc->id);
    table = i_form_table_create (reqdata->form_out, id_str, NULL, 3);
    free (id_str);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 0, 0, "142", 4);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 1, 0, "15%", 4);
    i_form_item_add_option (table, ITEM_OPTION_NOHEADERS, 0, 0, 0, NULL, 0);

    /* Start */
    labels[1] = "Incident ID";
    asprintf (&labels[2], "%li", inc->id);
    row = i_form_table_add_row (table, labels);
    free (labels[2]);

    /* Container */
    labels[1] = "Trigger";
    asprintf (&labels[2], "%s %s %s %s",
      inc->ent->cnt_desc, inc->ent->obj_desc, inc->ent->met_desc, inc->ent->trg_desc);
    row = i_form_table_add_row (table, labels);
    free (labels[2]);

    /* Start */
    labels[1] = "Start Time";
    labels[2] = i_time_ctime (inc->start_tv.tv_sec);
    row = i_form_table_add_row (table, labels);
    free (labels[2]);

    /* State */
    labels[1] = "Incident State";
    labels[2] = i_incident_statestr (inc->state);
    row = i_form_table_add_row (table, labels);

    /* Spacer */
    i_form_spacer_add (reqdata->form_out);
  }
  
  i_form_frame_end (reqdata->form_out, "uhincidentlist_frame");
   
  /*
   * Render Handled Incident List
   */
  
  i_form_frame_start (reqdata->form_out, "incidentlist_frame", "All Active  Incidents");
   
  /* Iterate incident list */
  inc_list = l_incident_list ();
  for (i_list_move_head(inc_list); (inc=i_list_restore(inc_list))!=NULL; i_list_move_next(inc_list))
  {
    int row;
    char *id_str;
    char *inc_str;
    char *headline;
    char *labels[3] = { "", "", "" };
    i_form_item *fstr;
    i_form_item *table;

    /* Add headline string */
    asprintf (&inc_str, "%s Incident", i_incident_typestr (inc->type));
    asprintf (&id_str, "%li_headline", inc->id);
    asprintf (&headline, "%s at %s", inc->ent->dev_desc, inc->ent->site_desc);
    fstr = i_form_string_add (reqdata->form_out, id_str, inc_str, headline);
    free (inc_str);
    free (id_str);
    free (headline);
    i_form_string_set_fgcolor (fstr, i_colour_fg_str(inc->ent->opstate), i_colour_fg_str(inc->ent->opstate));

    /* Create table */
    asprintf (&id_str, "%li_table", inc->id);
    table = i_form_table_create (reqdata->form_out, id_str, NULL, 3);
    free (id_str);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 0, 0, "142", 4);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 1, 0, "15%", 4);
    i_form_item_add_option (table, ITEM_OPTION_NOHEADERS, 0, 0, 0, NULL, 0);
                
    /* Start */
    labels[1] = "Incident ID";
    asprintf (&labels[2], "%li", inc->id);
    row = i_form_table_add_row (table, labels);
    free (labels[2]);

    /* Container */
    labels[1] = "Trigger";
    asprintf (&labels[2], "%s %s %s %s", 
      inc->ent->cnt_desc, inc->ent->obj_desc, inc->ent->met_desc, inc->ent->trg_desc);
    row = i_form_table_add_row (table, labels);
    free (labels[2]);

    /* Start */
    labels[1] = "Start Time";
    labels[2] = i_time_ctime (inc->start_tv.tv_sec);
    row = i_form_table_add_row (table, labels);
    free (labels[2]);

    /* State */
    labels[1] = "Incident State";
    labels[2] = i_incident_statestr (inc->state);
    row = i_form_table_add_row (table, labels);

    /* Spacer */
    i_form_spacer_add (reqdata->form_out);
  }

  i_form_frame_end (reqdata->form_out, "incidentlist_frame");

  return 1;
}

