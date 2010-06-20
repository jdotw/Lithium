#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "interface.h"
#include "ip.h"
#include "list.h"
#include "data.h"
#include "metric.h"

/* Struct Manipulation */

i_interface* i_interface_create ()
{
  i_interface *iface;

  iface = (i_interface *) malloc (sizeof(i_interface));
  if (!iface)
  {
    i_printf (1, "i_interface_create failed to malloc iface");
    return NULL;
  }
  memset (iface, 0, sizeof(i_interface));

  return iface;
}

void i_interface_free (void *ifaceptr)
{
  i_interface *iface = ifaceptr;

  if (!iface) return;

  if (iface->ip_list) i_list_free (iface->ip_list);
  if (iface->route_list) i_list_free (iface->route_list);
  if (iface->tput_cg) i_metric_cgraph_free (iface->tput_cg);

  free (iface);
}

/* Bytecount struct Manipulation */

i_interface_bytecount* i_interface_bytecount_create ()
{
  i_interface_bytecount *bc;

  bc = (i_interface_bytecount *) malloc (sizeof(i_interface_bytecount));
  if (!bc)
  { i_printf (1, "i_interface_bytecount_create failed to malloc bc struct"); return NULL; }
  memset (bc, 0, sizeof(i_interface_bytecount));

  return bc;
}

void i_interface_bytecount_free (void *bcptr)
{
  i_interface_bytecount *bc = bcptr;

  if (bc->site_id) free (bc->site_id);
  if (bc->device_id) free (bc->device_id);
  if (bc->ifdescr) free (bc->ifdescr);

  free (bc);
}   

/* State String */

char* i_interface_state_str (int state)
{
  char *state_str;
  
  switch (state)
  {
    case -1:
    case 0: state_str = strdup ("N/A");
            break;
    case 1: state_str = strdup ("Up");
            break;
    case 2: state_str = strdup ("Down");
            break;
    case 5: state_str = strdup ("Dormant");
            break;
    default: asprintf (&state_str, "%i", state);
  }

  return state_str;
}

/* FIX LEGACY STUB */

i_interface* i_interface_data_to_struct (char *data, int datasize)
{ return NULL; }

char* i_interface_struct_to_data (i_interface *iface, int *datasizeptr)
{
  *datasizeptr = 0;
  return NULL;
}
