#include <stdlib.h>

#include "induction.h"
#include "threshold.h"
#include "navform.h"
#include "list.h"
#include "configfile.h"
#include "files.h"
#include "auth.h"

static int static_enabled = 0;

/* Enable / Disable */

int i_threshold_enable (i_resource *self)
{
  int num;
  char *res_addr_str;

  if (static_enabled == 1)
  { i_printf (1, "i_threshold_enable warning, sub-system already enabled"); return 0; }

  static_enabled = 1;

  num = i_threshold_sql_enable (self);
  if (num != 0)
  { i_printf (1, "i_threshold_enable failed to enable threshold_sql sub-system"); i_threshold_disable (self); return -1; }

  num = i_threshold_section_enable (self);
  if (num != 0)
  { i_printf (1, "i_threshold_enable failed to enable threshold_section sub-system"); i_threshold_disable (self); return -1; }

  res_addr_str = i_resource_address_struct_to_string (RES_ADDR(self));
  num = i_navform_link_add (self, "thresholds", "Thresholds", NAVFORM_THRESHOLDS, AUTH_LEVEL_CLIENT, res_addr_str, "threshold_main", NULL, 0);
  free (res_addr_str);
  if (num != 0)
  { i_printf (1, "i_threshold_enable failed to add navform link"); i_threshold_disable (self); return -1; }

  return 0;
}

int i_threshold_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "i_threshold_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  i_threshold_sql_disable (self);
  i_threshold_section_disable (self);

  i_navform_link_remove (self, "thresholds");

  return 0;
}

/* Struct Manipulation */

i_threshold* i_threshold_create ()
{
  i_threshold *thold;

  thold = (i_threshold *) malloc (sizeof(i_threshold));
  if (!thold)
  { i_printf (1, "i_threshold_create failed to malloc thold struct"); return NULL; }
  memset (thold, 0, sizeof(i_threshold));

  return thold;
}

void i_threshold_free (void *tholdptr)
{
  i_threshold *thold = tholdptr;

  if (!thold) return;

  if (thold->name) free (thold->name);
  if (thold->object_desc) free (thold->object_desc);
  if (thold->value_desc) free (thold->value_desc);

  free (thold);
} 

/* Register / Deregister */

i_threshold* i_threshold_register (i_resource *self, i_threshold_section *section, char *name, char *object_desc, char *value_desc, float alert_default, float critical_default, float tolerance_default)
{
  int num;
  char *str;
  i_threshold *thold;

  if (!self || !section || !name || !object_desc || !value_desc) return NULL;

  /* Check the THRESHOLDCONF_FILE for defaults which 
   * override any passed to this func 
   */

  str = i_configfile_get (self, THRESHOLDCONF_FILE, name, "alert_value", 0);
  if (str)
  { alert_default = atof (str); free (str); }
  str = i_configfile_get (self, THRESHOLDCONF_FILE, name, "critical_value", 0);
  if (str)
  { critical_default = atof (str); free (str); }
  str = i_configfile_get (self, THRESHOLDCONF_FILE, name, "tolerance_percent", 0);
  if (str)
  { tolerance_default = atof (str); free (str); }

  /* Attempt to find the threshold in the SQL db */

  thold = i_threshold_sql_get (self, section, name);
  if (thold)
  {
    /* Existing threshold, check the defaults are accurate */
    
    if (thold->alert_default != alert_default)
    {
      if (thold->alert_value == thold->alert_default) 
      { thold->alert_value = alert_default; }
      thold->alert_default = alert_default;
      i_threshold_sql_update (self, thold);
    }

    if (thold->critical_default != critical_default)
    {
      if (thold->critical_value == thold->critical_default) 
      { thold->critical_value = critical_default; }
      thold->critical_default = critical_default;
      i_threshold_sql_update (self, thold);
    }

    if (thold->tolerance_default != tolerance_default)
    { 
      if (thold->tolerance_percent == thold->tolerance_default)
      { thold->tolerance_percent = tolerance_default; }
      thold->tolerance_default = tolerance_default;
      i_threshold_sql_update (self, thold);
    }

    /* Free/Set the descriptions */

    if (thold->object_desc)
    { free (thold->object_desc); }
    thold->object_desc = strdup (object_desc);

    if (thold->value_desc)
    { free (thold->value_desc); }
    thold->value_desc = strdup (value_desc);
  }
  else
  {
    /* New threshold */

    thold = i_threshold_create ();
    if (!thold)
    { i_printf (1, "i_threshold_register failed to create thold struct"); return NULL; }
    thold->name = strdup (name);
    thold->object_desc = strdup (object_desc);
    thold->value_desc = strdup (value_desc);
    thold->alert_value = alert_default;
    thold->alert_default = alert_default;
    thold->critical_value = critical_default;
    thold->critical_default = critical_default;
    thold->tolerance_percent = tolerance_default;
    thold->tolerance_default = tolerance_default;
    thold->section = section;

    num = i_threshold_sql_put (self, thold);
    if (num != 0)
    { i_printf (1, "i_threshold_register failed to put threshold in SQL database"); i_threshold_free (thold); return NULL; }
  }

  num = i_list_enqueue (section->thresholds, thold);
  if (num != 0)
  { i_printf (1, "i_threshold_register failed to enqueue thold into section->thresholds"); i_threshold_free (thold); return NULL; }

  return thold;
}

int i_threshold_deregister (i_threshold_section *section, char *name)
{
  int delete_count = 0;
  i_threshold *thold;

  if (!section || !name)
  { return -1; }
  
  for (i_list_move_head(section->thresholds); (thold=i_list_restore(section->thresholds))!=NULL; i_list_move_next(section->thresholds))
  {
    if (!strcmp(section->name, name))
    { i_list_delete (section->thresholds); delete_count++; }
  }

  if (delete_count < 1) return -1;

  return 0;
}

/* Get */

i_threshold* i_threshold_get (i_resource *self, i_threshold_section *section, char *name)
{
  i_threshold *thold;

  if (!self || !section || !name)
  { return NULL; }

  for (i_list_move_head(section->thresholds); (thold=i_list_restore(section->thresholds))!=NULL; i_list_move_next(section->thresholds))
  {
    if (!strcmp(thold->name, name))
    { return thold; }
  }

  return NULL;  
}

