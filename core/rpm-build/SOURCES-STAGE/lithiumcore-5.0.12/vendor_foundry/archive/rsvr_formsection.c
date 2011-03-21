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

#include "rsvr.h"
#include "util.h"

int v_rsvr_formsection (i_resource *self, i_form *form, void *passdata, int passdatasize, i_authentication *auth)
{
  /* ALWAYS RETURN 0 */
  
  int row;
  i_list *rsvr_list;
  
  rsvr_list = v_rsvr_list ();
  
  if (rsvr_list)
  {
    char *labels[5] = { "Server Name", "IP Address", "Port", "Administrative Status", "Operational Status" };
    i_form_item *table;
    v_rsvr *rsvr;

    table = i_form_table_create (form, "rsvr", NULL, 5);
    if (!table)
    { i_printf (1, "v_rsvr_formsection failed to create rsvr table"); return 0; }
    i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width */
  
    i_form_table_add_row (table, labels);

    /* Servers */
    for (i_list_move_head(rsvr_list); (rsvr=i_list_restore(rsvr_list))!=NULL; i_list_move_next(rsvr_list))
    {
      v_rsvr_port *port;

      labels[0] = rsvr->name_str;
      labels[1] = inet_ntoa (rsvr->ip_addr);
      labels[2] = "All";
      if (rsvr->admin_state_current == 1)
      { labels[3] = strdup (v_util_adminstate_str (rsvr->admin_state)); }
      else
      { asprintf (&labels[3], "%s *", v_util_adminstate_str (rsvr->admin_state)); }
      if (rsvr->op_state_current == 1)
      { labels[4] = strdup (v_util_opstate_str (rsvr->op_state)); }
      else
      { asprintf (&labels[4], "%s *", v_util_opstate_str (rsvr->op_state)); }
      
      row = i_form_table_add_row (table, labels);

      free (labels[3]);
      free (labels[4]);

      /* Server ports */
      for (i_list_move_head(rsvr->port_list); (port=i_list_restore(rsvr->port_list))!=NULL; i_list_move_next(rsvr->port_list))
      {
        labels[0] = rsvr->name_str;
        labels[1] = inet_ntoa (rsvr->ip_addr);
        asprintf (&labels[2], "%lu", port->port);
        if (port->admin_state_current == 1)
        { labels[3] = strdup (v_util_adminstate_str (port->admin_state)); }
        else
        { asprintf (&labels[3], "%s *", v_util_adminstate_str (port->admin_state)); }
        if (port->op_state_current == 1)
        { labels[4] = strdup (v_util_opstate_str (port->op_state)); }
        else
        { asprintf (&labels[4], "%s *", v_util_opstate_str (port->op_state)); }
        
        row = i_form_table_add_row (table, labels);

        free (labels[2]);
        free (labels[3]);
        free (labels[4]);
      }
    }

    i_form_string_add (form, "rsvr_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  { i_form_string_add (form, "msg", "Real Server List", "List not yet populated"); }

  return 0;
}
