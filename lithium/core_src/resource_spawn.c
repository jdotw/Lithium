#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/message.h>
#include <induction/socket.h>
#include <induction/data.h>
#include <induction/respond.h>
#include <induction/cement.h>
#include <induction/entity.h>

extern i_hashtable *global_res_table;

int l_resource_spawn_handler (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Process an incomming resource creation request
   *
   * Always return 0 to keep the handler active
   */

  int type;
  int ident_int;
  int entdata_size;
  int offset;
  long msgid;
  char *ident_str;
  char *module_name;
  char *entdata;
  char *dataptr;
  char *addr_str;
  char *root;
  char *customer_id;
  i_resource *res = passdata;
  i_resource *new_res;
  i_entity *ent;

  if (!self || !msg || !msg->data || !res) return 0;

  dataptr = msg->data;

  /* Get type */
  type = i_data_get_int (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1) { i_printf (1, "l_resource_spawn_handler unable to get type from data"); i_respond_failed (res, msg, 0); return 0; }
  dataptr += offset;

  /* Get ident_int */
  ident_int = i_data_get_int (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1) { i_printf (1, "l_resource_spawn_handler unable to get ident_int from data"); i_respond_failed (res, msg, 0); return 0; }
  dataptr += offset;

  /* Get ident_str */
  ident_str = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1) { i_printf (1, "l_resource_spawn_handler unable to get ident_str from data"); i_respond_failed (res, msg, 0); return 0; }
  dataptr += offset;

  /* Get module_name */
  module_name = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  {
    i_printf (1, "l_resource_spawn_handler unable to get module_name from data");
    if (ident_str) free (ident_str);
    i_respond_failed (res, msg, 0);
    return 0;
  }
  dataptr += offset;

  /* Get root */
  root = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  {
    i_printf (1, "l_resource_spawn_handler unable to get root from data");
    if (ident_str) free (ident_str);
    if (module_name) free (module_name);
    i_respond_failed (res, msg, 0);
    return 0;
  }
  dataptr += offset;

  /* Get customer ID */
  customer_id = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  {
    i_printf (1, "l_resource_spawn_handler unable to get customer_id from data");
    if (ident_str) free (ident_str);
    if (module_name) free (module_name);
    if (root) free (root);
    i_respond_failed (res, msg, 0);
    return 0;
  }
  dataptr += offset;

  /* Get entity data */
  entdata = i_data_get_chunk (msg->data, dataptr, msg->datasize, &entdata_size, &offset);
  if (offset < 1)
  {
    i_printf (1, "l_resource_spawn_handler unable to get entdata from data");
    if (ident_str) free (ident_str);
    if (module_name) free (module_name);
    if (root) free (root);
    if (customer_id) free (customer_id);
    i_respond_failed (res, msg, 0);
    return 0;
  }
  dataptr += offset;

  /* Convert entity data to entity */
  if (entdata)
  {
    ent = i_entity_struct (entdata, entdata_size);
    free (entdata);
    if (!ent)
    {
      i_printf (1, "l_resource_spawn_handler unable to convert provided config_form_data into config_form");
      if (ident_str) free (ident_str);
      if (module_name) free (module_name);
      if (root) free (root);
      if (customer_id) free (customer_id);
      i_respond_failed (res, msg, 0);
      return 0;
    }
  }
  else ent = NULL;

  /* Create local resouce */
  new_res = i_resource_local_create (self, msg->src, global_res_table, type, ident_int, ident_str, module_name, root, customer_id, ent);
  if (ident_str) free (ident_str);
  if (module_name) free (module_name);
  if (root) free (root);
  if (customer_id) free (customer_id);
  if (ent) 
  {
    /* Recursively find parent (root) entity */
    while (ent->parent)
    { ent = ent->parent; }

    /* Free root parent entity */
    i_entity_free (ent);
  }
  if (!new_res)
  {
    i_printf (1, "l_resource_spawn_handler unable to create resource");
    i_respond_failed (res, msg, 0);
    return 0;
  }

  /* Create address string for message */
  addr_str = i_resource_address_struct_to_string (RES_ADDR(new_res));
  if (!addr_str)
  {
    i_printf (1, "l_resource_spawn_handler unable to convert res to addr_str");
    i_resource_local_terminate (global_res_table, RES_ADDR(new_res));
    i_resource_local_remove (self, global_res_table, RES_ADDR(new_res));
    i_respond_failed (res, msg, 0);
    return 0;
  }

  /* Send resource address string response */
  msgid = i_message_send (res, MSG_RES_SPAWN, addr_str, strlen(addr_str)+1, msg->src, MSG_FLAG_RESP, msg->msgid);
  free (addr_str);
  if (msgid == -1)
  {
    i_printf (1, "l_resource_spawn_handler unable to send response msg");
    i_resource_local_terminate (global_res_table, RES_ADDR(new_res));
    i_resource_local_remove (self, global_res_table, RES_ADDR(new_res));
    i_respond_failed (res, msg, 0);
    return 0;
  }

  /* Announce */
  i_printf (2, "l_resource_spawn_handler successfully created %s:%s:%i:%i:%s for %s:%s:%i:%i:%s",
    new_res->plexus, new_res->node, new_res->type, new_res->ident_int, new_res->ident_str,
    msg->src->plexus, msg->src->node, msg->src->type, msg->src->ident_int, msg->src->ident_str);

  return 0;
}

