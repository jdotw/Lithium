#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/threshold.h>
#include <induction/status.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/mainform.h>
#include <induction/navform.h>
#include <lithium/snmp.h>

#include "rsvr.h"

static int static_enabled = 0;

/* Enable / Disable */

int v_rsvr_enable (i_resource *self)
{
  int num;
  
  if (static_enabled == 1)
  { i_printf (1, "v_rsvr_enable warning, v_rsvr sub-system already enabled"); return 0; }

  static_enabled = 1;

  num = i_mainform_section_add (self, "rsvr", "Real Servers", v_rsvr_formsection, MAINFORM_AVAIL+2);
  if (num != 0)
  { i_printf (1, "v_rsvr_enable failed to add the rsvr section to the mainform"); v_rsvr_disable (self); return -1; }
    
  num = v_rsvr_list_populate (self);
  if (num != 0)
  { i_printf (1, "v_rsvr_enable failed to call v_rsvr_list_populate"); v_rsvr_disable (self); return -1; }

  num = v_rsvr_refresh_enable (self);
  if (num != 0)
  { i_printf (1, "v_rsvr_enable faled to enable v_rsvr_refresh"); v_rsvr_disable (self); return -1; }

  return 0;
}

int v_rsvr_disable (i_resource *self)
{
  int num; 

  if (static_enabled == 0)
  { i_printf (1, "v_rsvr_disable warning, v_rsvr sub-system already disabled"); return 0; }

  static_enabled = 0;

  num = i_mainform_section_remove (self, "rsvr");
  if (num != 0)
  { i_printf (1, "v_rsvr_disable failed to call i_mainform_section_remove for rsvr section"); }

  num = v_rsvr_list_populate_terminate (self);
  if (num != 0)
  { i_printf (1, "v_rsvr_disable failed to call v_rsvr_list_populate_terminate"); }

  num = v_rsvr_refresh_disable (self); 
  if (num != 0)
  { i_printf (1, "v_rsvr_disable failed to call v_rsvr_refresh_disable"); }

  return 0;
}

/* Struct manipulation */

v_rsvr* v_rsvr_create ()
{
  v_rsvr *rsvr;

  rsvr = (v_rsvr *) malloc (sizeof(v_rsvr));
  if (!rsvr)
  { i_printf (1, "v_rsvr_create failed to malloc rsvr struct"); return NULL; }
  memset (rsvr, 0, sizeof (v_rsvr));

  rsvr->port_list = i_list_create ();
  if (!rsvr->port_list)
  { i_printf (1, "v_rsvr_create failed to create rsvr->port_list"); v_rsvr_free (rsvr); return NULL; }
  i_list_set_destructor (rsvr->port_list, v_rsvr_port_free);
  
  return rsvr;
}

void v_rsvr_free (void *rsvrptr)
{
  v_rsvr *rsvr = rsvrptr;

  if (!rsvr) return;

  if (rsvr->name_str) free (rsvr->name_str);
  if (rsvr->port_list) i_list_free (rsvr->port_list);

  free (rsvr);
}
