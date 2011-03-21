#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/inventory.h>

#include "inventory.h"

/* Remove static item */

int form_inventory_static_remove (i_resource *self, i_form_reqdata *reqdata)
{
  char *dev_addr_str;
  char *invitemptr_str;
  /* Form setup */

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_inventory_static_remove failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "staticremove", "Inventory - Remove Static Item");

  if (!reqdata->form_passdata || reqdata->form_passdata_size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No device/invitem specified"); return 1; }

  dev_addr_str = (char *) reqdata->form_passdata;
  invitemptr_str = strrchr (dev_addr_str, ':');
  if (!invitemptr_str)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Passdata is corrupt"); return 1; }
  *invitemptr_str = '\0';
  invitemptr_str++;

  l_inventory_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  i_form_string_add (reqdata->form_out, "confirm", "Confirm", "Please confirm static item removal");
  
  i_form_hidden_add (reqdata->form_out, "dev_addr_str", dev_addr_str);
  i_form_hidden_add (reqdata->form_out, "invitemptr_str", invitemptr_str);

  i_form_option_add (reqdata->form_out, FORM_OPTION_SUBMIT_LABEL, "Confirm", 8);

  i_form_frame_end (reqdata->form_out, "staticremove");

  return 1;
}

int form_inventory_static_remove_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  char *invitemptr_str;
  char *dev_addr_str;
  void *invitemptr;
  i_list *list;
  i_form_item_option *opt;
  i_inventory_item *invitem;
  
  /* Form setup */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_inventory_static_remove_submit failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "staticremove", "Inventory - Remove Static Item");

  /* Get dev_addr_str */
  opt = i_form_get_value_for_item (reqdata->form_in, "dev_addr_str");
  if (opt)
  { dev_addr_str = (char *) opt->data; }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find dev_addr_str in form"); return 1; }

  /* Get invitemptr_str */
  opt = i_form_get_value_for_item (reqdata->form_in, "invitemptr_str");
  if (opt)
  { invitemptr_str = (char *) opt->data; }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find invitemptr_str in form"); return 1; }
  invitemptr = (void *) strtoul (invitemptr_str, NULL, 16);

  /* Find item */
  list = l_inventory_get_device_str (self, dev_addr_str);
  
  for (i_list_move_head(list); (invitem=i_list_restore(list))!=NULL; i_list_move_next(list))
  { if (invitem == invitemptr) break; }

  if (!invitem)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified inventory item not found"); return 1; }
  if (!(invitem->flags & INV_FLAG_STATIC))
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified inventory item is not a static item"); return 1; }

  /* Remove item */
  num = l_inventory_static_remove_item (self, invitem);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to remove static item from inventory"); return 1; }

  /* Success */
  i_form_string_add (reqdata->form_out, "msg", "Success", "Successully removed static item from inventory");

  i_form_frame_end (reqdata->form_out, "staticremove");

  return 1;
}
