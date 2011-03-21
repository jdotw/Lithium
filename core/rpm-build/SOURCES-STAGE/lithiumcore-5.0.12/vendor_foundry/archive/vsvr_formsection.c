#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
#include <lithium/snmp.h>

#include "vsvr.h"
#include "util.h"

int v_vsvr_formsection (i_resource *self, i_form *form, void *passdata, int passdatasize, i_authentication *auth)
{
  /* ALWAYS RETURN 0 */
  
  int row;
  i_list *vsvr_list;
  
  vsvr_list = v_vsvr_list ();
  
  if (vsvr_list)
  {
    char *labels[4] = { "Server Name", "IP Address", "Port", "Administrative Status" };
    i_form_item *table;
    v_vsvr *vsvr;

    table = i_form_table_create (form, "vsvr", NULL, 4);
    if (!table)
    { i_printf (1, "v_vsvr_formsection failed to create vsvr table"); return 0; }
    i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width */
  
    i_form_table_add_row (table, labels);

    /* Servers */
    for (i_list_move_head(vsvr_list); (vsvr=i_list_restore(vsvr_list))!=NULL; i_list_move_next(vsvr_list))
    {
      v_vsvr_port *port;

      labels[0] = vsvr->name_str;
      labels[1] = inet_ntoa (vsvr->ip_addr);
      labels[2] = "All";
      if (vsvr->admin_state_current == 1)
      { labels[3] = strdup (v_util_adminstate_str (vsvr->admin_state)); }
      else
      { asprintf (&labels[3], "%s *", v_util_adminstate_str (vsvr->admin_state)); }
      
      row = i_form_table_add_row (table, labels);

      free (labels[3]);

      /* Server ports */
      for (i_list_move_head(vsvr->port_list); (port=i_list_restore(vsvr->port_list))!=NULL; i_list_move_next(vsvr->port_list))
      {
        labels[0] = vsvr->name_str;
        labels[1] = inet_ntoa (vsvr->ip_addr);
        asprintf (&labels[2], "%lu", port->port);
        if (port->admin_state_current == 1)
        { labels[3] = strdup (v_util_adminstate_str (port->admin_state)); }
        else
        { asprintf (&labels[3], "%s *", v_util_adminstate_str (port->admin_state)); }
        
        row = i_form_table_add_row (table, labels);

        free (labels[2]);
        free (labels[3]);
      }
    }

    i_form_string_add (form, "vsvr_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  { i_form_string_add (form, "msg", "Virtual Server List", "List not yet populated"); }

  return 0;
}
