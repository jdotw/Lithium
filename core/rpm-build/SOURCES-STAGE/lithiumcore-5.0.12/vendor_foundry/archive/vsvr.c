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
#include <induction/inventory.h>
#include <induction/mainform.h>
#include <induction/navform.h>
#include <lithium/snmp.h>

#include "vsvr.h"

static int static_enabled = 0;

/* Enable / Disable */

int v_vsvr_enable (i_resource *self)
{
  int num;
  
  if (static_enabled == 1)
  { i_printf (1, "v_vsvr_enable warning, v_vsvr sub-system already enabled"); return 0; }

  static_enabled = 1;

  num = i_mainform_section_add (self, "vsvr", "Virtual Servers", v_vsvr_formsection, MAINFORM_AVAIL+1);
  if (num != 0)
  { i_printf (1, "v_vsvr_enable failed to add the vsvr section to the mainform"); v_vsvr_disable (self); return -1; }

  num = v_vsvr_list_populate (self);
  if (num != 0)
  { i_printf (1, "v_vsvr_enable failed to call v_vsvr_list_populate"); v_vsvr_disable (self); return -1; }

  num = v_vsvr_refresh_enable (self);
  if (num != 0)
  { i_printf (1, "v_vsvr_enable faled to enable v_vsvr_refresh"); v_vsvr_disable (self); return -1; }

  return 0;
}

int v_vsvr_disable (i_resource *self)
{
  int num; 

  if (static_enabled == 0)
  { i_printf (1, "v_vsvr_disable warning, v_vsvr sub-system already disabled"); return 0; }

  static_enabled = 0;

  num = i_mainform_section_remove (self, "vsvr");
  if (num != 0)
  { i_printf (1, "v_vsvr_disable failed to call i_mainform_section_remove for vsvr section"); }

  num = v_vsvr_list_populate_terminate (self);
  if (num != 0)
  { i_printf (1, "v_vsvr_disable failed to call v_vsvr_list_populate_terminate"); }

  num = v_vsvr_refresh_disable (self); 
  if (num != 0)
  { i_printf (1, "v_vsvr_disable failed to call v_vsvr_refresh_disable"); }

  return 0;
}

/* Struct manipulation */

v_vsvr* v_vsvr_create ()
{
  v_vsvr *vsvr;

  vsvr = (v_vsvr *) malloc (sizeof(v_vsvr));
  if (!vsvr)
  { i_printf (1, "v_vsvr_create failed to malloc vsvr struct"); return NULL; }
  memset (vsvr, 0, sizeof (v_vsvr));

  vsvr->port_list = i_list_create ();
  if (!vsvr->port_list)
  { i_printf (1, "v_vsvr_create failed to create vsvr->port_list"); v_vsvr_free (vsvr); return NULL; }
  i_list_set_destructor (vsvr->port_list, v_vsvr_port_free);
  
  return vsvr;
}

void v_vsvr_free (void *vsvrptr)
{
  v_vsvr *vsvr = vsvrptr;

  if (!vsvr) return;

  if (vsvr->name_str) free (vsvr->name_str);
  if (vsvr->port_list) i_list_free (vsvr->port_list);

  free (vsvr);
}
