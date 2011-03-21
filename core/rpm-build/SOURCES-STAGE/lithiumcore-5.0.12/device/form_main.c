#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timeutil.h>
#include <induction/hierarchy.h>
#include <induction/vendor.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>

#include "devform.h"
#include "metform.h"
#include "trgform.h"

int form_main (i_resource *self, i_form_reqdata *reqdata)
{ 
  int retval = 1;
  char *str;
  char *labels[4] = { "Site", "Device Description", "Management IP", "Vendor Module" };
  i_vendor *vendor;
  i_form_item *table;
  i_entity *ent = NULL;

  /* 
   * Form Setup 
   */
  
  /* Check auth */
  if (!reqdata || !reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create form */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_main failed to create form"); return -1; }

  /* Start Frame */
  asprintf (&str,  "%s (%s)", self->hierarchy->dev->desc_str, self->hierarchy->dev->name_str);
  i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
  free (str);

  /* Get local entity */
  if (reqdata->entaddr)
  {
    ent = i_entity_local_get (self, reqdata->entaddr);
    if (!ent)
    {
      i_form_string_add (reqdata->form_out, "error", "Error", "Specified entity not found"); 
      i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);
      return 1;
    }
  }
  else
  { ent = ENTITY(self->hierarchy->dev); }

  /* 
   * Basic device info frame 
   */

  /* Create Table */
  table = i_form_table_create (reqdata->form_out, "deviceinfo_table", NULL, 4);
  if (!table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create table for basic device info"); return -1; }
  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  /* Add header row */
  i_form_table_add_row (table, labels);

  /* Set labels */
  if (self->hierarchy)
  {
    if (self->hierarchy->site_desc) labels[0] = self->hierarchy->site_desc;
    if (self->hierarchy->device_desc) labels[1] = self->hierarchy->device_desc;
    if (self->hierarchy->dev && self->hierarchy->dev->ip_str) labels[2] = self->hierarchy->dev->ip_str;
  }
  vendor = i_vendor_get ();
  if (vendor)
  { labels[3] = vendor->name_str; }
  else
  { labels[3] = "None Loaded"; }

  /* Add data label row */
  i_form_table_add_row (table, labels);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);

  /*
   * Mainform Handling
   */

  /* Check for pre-set form func */
  if (ent->mainform_func)
  { 
    retval = ent->mainform_func (self, ent, reqdata); 
  }
  else
  {
    /* Entity-specific default handling */
    switch (ent->ent_type)
    {
      case ENT_DEVICE:
        retval = l_devform_generate (self, ent, reqdata);
        break;
      case ENT_CONTAINER:
        break;
      case ENT_OBJECT:
        break;
      case ENT_METRIC:
        retval = l_metform_generate (self, ent, reqdata);
        break;
      case ENT_TRIGGER:
        retval = l_trgform_generate (self, ent, reqdata);
        break;
    }
  }

  /* 
   * Historic Data handling
   */

  /* Historic data form handling */
  if (ent->ent_type == ENT_OBJECT || ent->ent_type == ENT_METRIC)
  {
    int (*histform_func) () = NULL;

    if (ent->ent_type == ENT_OBJECT)
    {
      i_object *obj = (i_object *) ent;
      histform_func = obj->histform_func;
    }
    else
    {
      i_metric *met = (i_metric *) ent;
      if (!met->histform_func && !met->mainform_func && met->record_method != RECMETHOD_NONE)
      { histform_func = l_metform_hist_generate; }
      else
      { histform_func = met->histform_func; }
    }

    if (histform_func)
    { 
      int histretval;
      i_form_item *item;
      i_entity *tmpent;
      struct tm ref_tm;

      /* Set Submit */
      i_form_set_submit (reqdata->form_out, 1);

      /* Start Data Frame */
      asprintf (&str, "%s_historic", ent->name_str);
      i_form_frame_start (reqdata->form_out, str, "Historic Data");
      free (str);

      if (reqdata->ref_sec == 0)
      { str = i_time_ctime (time(NULL)); }
      else
      { str = i_time_ctime (reqdata->ref_sec); }
      i_form_string_add (reqdata->form_out, "select_note", "Reference Time", str); 
      free (str);
      i_form_string_add (reqdata->form_out, "select_note", "Note", "Scroll down to select time and entity reference"); 
      i_form_spacer_add (reqdata->form_out);

      /* Call Func */
      histretval = histform_func (self, ent, reqdata);

      /* End Data Frame */
      asprintf (&str, "%s_historic", ent->name_str);
      i_form_frame_end (reqdata->form_out, str);
      free (str);

      /* Start Selector Frame */
      i_form_frame_start (reqdata->form_out, "refsec_selector", "Reference Time/Entity Selection");

      /* Entity Selector */
      item = i_form_dropdown_create ("select_entaddr", i_entity_typestr (ent->ent_type));
      for (i_list_move_head(ent->parent->child_list); (tmpent=i_list_restore(ent->parent->child_list))!=NULL; i_list_move_next(ent->parent->child_list))
      {
        char *entaddrstr;
        entaddrstr = i_entity_address_string (tmpent, NULL);
        if (tmpent->desc_str)
        { i_form_dropdown_add_option (item, entaddrstr, tmpent->desc_str, 0); }
        else
        { i_form_dropdown_add_option (item, entaddrstr, tmpent->name_str, 0); }
        if (!strcmp(tmpent->name_str, ent->name_str))
        { i_form_dropdown_set_selected (item, entaddrstr); }
        free (entaddrstr);
      }
      i_form_add_item (reqdata->form_out, item);

      /* Get tm for current reqdata->ref_sec */
      if (reqdata->ref_sec == 0)
      {
        time_t now_sec;
        now_sec = time (NULL);
        gmtime_r (&now_sec, &ref_tm);
      }
      else
      { gmtime_r (&reqdata->ref_sec, &ref_tm); }
      
      /* Day Selector */
      item = i_form_dropdown_create ("select_reftime_day", "Day");
      i_form_dropdown_add_option (item, "1", "1", 0);
      i_form_dropdown_add_option (item, "2", "2", 0);
      i_form_dropdown_add_option (item, "3", "3", 0);
      i_form_dropdown_add_option (item, "4", "4", 0);
      i_form_dropdown_add_option (item, "5", "5", 0);
      i_form_dropdown_add_option (item, "6", "6", 0);
      i_form_dropdown_add_option (item, "7", "7", 0);
      i_form_dropdown_add_option (item, "8", "8", 0);
      i_form_dropdown_add_option (item, "9", "9", 0);
      i_form_dropdown_add_option (item, "10", "10", 0);
      i_form_dropdown_add_option (item, "11", "11", 0);
      i_form_dropdown_add_option (item, "12", "12", 0);
      i_form_dropdown_add_option (item, "13", "13", 0);
      i_form_dropdown_add_option (item, "14", "14", 0);
      i_form_dropdown_add_option (item, "15", "15", 0);
      i_form_dropdown_add_option (item, "16", "16", 0);
      i_form_dropdown_add_option (item, "17", "17", 0);
      i_form_dropdown_add_option (item, "18", "18", 0);
      i_form_dropdown_add_option (item, "19", "19", 0);
      i_form_dropdown_add_option (item, "20", "20", 0);
      i_form_dropdown_add_option (item, "21", "21", 0);
      i_form_dropdown_add_option (item, "22", "22", 0);
      i_form_dropdown_add_option (item, "23", "23", 0);
      i_form_dropdown_add_option (item, "24", "24", 0);
      i_form_dropdown_add_option (item, "25", "25", 0);
      i_form_dropdown_add_option (item, "26", "26", 0);
      i_form_dropdown_add_option (item, "27", "27", 0);
      i_form_dropdown_add_option (item, "28", "28", 0);
      i_form_dropdown_add_option (item, "29", "29", 0);
      i_form_dropdown_add_option (item, "30", "30", 0);
      i_form_dropdown_add_option (item, "31", "31", 0);
      asprintf (&str, "%i", ref_tm.tm_mday);
      i_form_dropdown_set_selected (item, str);
      free (str);
      i_form_add_item (reqdata->form_out, item);

      /* Month Selector */
      item = i_form_dropdown_create ("select_reftime_month", "Month");
      i_form_dropdown_add_option (item, "0", i_time_month_string (0), 0);
      i_form_dropdown_add_option (item, "1", i_time_month_string (1), 0);
      i_form_dropdown_add_option (item, "2", i_time_month_string (2), 0);
      i_form_dropdown_add_option (item, "3", i_time_month_string (3), 0);
      i_form_dropdown_add_option (item, "4", i_time_month_string (4), 0);
      i_form_dropdown_add_option (item, "5", i_time_month_string (5), 0);
      i_form_dropdown_add_option (item, "6", i_time_month_string (6), 0);
      i_form_dropdown_add_option (item, "7", i_time_month_string (7), 0);
      i_form_dropdown_add_option (item, "8", i_time_month_string (8), 0);
      i_form_dropdown_add_option (item, "9", i_time_month_string (9), 0);
      i_form_dropdown_add_option (item, "10", i_time_month_string (10), 0);
      i_form_dropdown_add_option (item, "11", i_time_month_string (11), 0);
      asprintf (&str, "%i", ref_tm.tm_mon);
      i_form_dropdown_set_selected (item, str);
      free (str);
      i_form_add_item (reqdata->form_out, item);

      /* Year Selector */
      item = i_form_dropdown_create ("select_reftime_year", "Year");
      i_form_dropdown_add_option (item, "2002", "2002", 0);
      i_form_dropdown_add_option (item, "2003", "2003", 0);
      i_form_dropdown_add_option (item, "2004", "2004", 0);
      i_form_dropdown_add_option (item, "2005", "2005", 0);
      i_form_dropdown_add_option (item, "2006", "2006", 0);
      i_form_dropdown_add_option (item, "2007", "2007", 0);
      i_form_dropdown_add_option (item, "2008", "2008", 0);
      i_form_dropdown_add_option (item, "2009", "2009", 0);
      i_form_dropdown_add_option (item, "2010", "2010", 0);
      i_form_dropdown_add_option (item, "2011", "2011", 0);
      asprintf (&str, "%i", ref_tm.tm_year+1900);
      i_form_dropdown_set_selected (item, str);
      free (str);
      i_form_add_item (reqdata->form_out, item);
      
      /* End Selector Frame */
      i_form_frame_end (reqdata->form_out, "refsec_selector");
    }
  }

  return retval;
}

int form_main_submit (i_resource *self, i_form_reqdata *reqdata)
{
  char *entaddrstr;
  struct timeval now;
  struct tm ref_tm;
  i_form_item_option *opt;

  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Initialise time */
  gettimeofday (&now, NULL);
  gmtime_r ((time_t *) &now.tv_sec, &ref_tm);
  ref_tm.tm_sec = 0;
  ref_tm.tm_min = 0;
  ref_tm.tm_hour = 0;

  /* Retrieve Values */
  
  opt = i_form_get_value_for_item (reqdata->form_in, "select_entaddr");
  if (opt)
  { entaddrstr = (char *) opt->data; }
  else
  { entaddrstr = NULL; }

  opt = i_form_get_value_for_item (reqdata->form_in, "select_reftime_day");
  if (opt)
  { ref_tm.tm_mday = atoi ((char *)opt->data); }

  opt = i_form_get_value_for_item (reqdata->form_in, "select_reftime_month");
  if (opt)
  { ref_tm.tm_mon = atol ((char *)opt->data); }

  opt = i_form_get_value_for_item (reqdata->form_in, "select_reftime_year");
  if (opt)
  { ref_tm.tm_year = atol ((char *)opt->data) - 1900; }

  reqdata->ref_sec = mktime (&ref_tm);

  /* Set Entity Address */
  if (entaddrstr)
  {
    if (reqdata->entaddr) i_entity_address_free (reqdata->entaddr);
    reqdata->entaddr = i_entity_address_struct (entaddrstr);
  }

  /* Return form_main */
  return form_main (self, reqdata);
}
