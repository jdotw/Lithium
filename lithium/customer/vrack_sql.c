#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/callback.h>
#include <induction/message.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/customer.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/incident.h>
#include <induction/postgresql.h>
#include <induction/xml.h>

#include "vrack.h"

/*
 * VRack Retrieval
 *
 * This is a four stage process:
 *
 * 1) Query the vracks table for basic info
 * 2) Query the vrack_devices table for the list of devices
 * 3) Query the vrack_cables table for the list of cables
 * 4) Query the vrack_cablegroups table for the list of cable groups
 *
 * Parse the output and fire a callback passing it the l_vrack struct
 */

i_callback* l_vrack_sql_get (i_resource *self, int doc_id, int (*cbfunc) (), void *passdata)
{
  int num;

  /* Callback */
  i_callback *cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Open conn */
  i_pg_async_conn *conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_vrack_sql_get failed to open SQL database connection"); return NULL; }

  /* Create query */
  char *query;
  asprintf (&query, "SELECT doc, ru_count, thumbnail FROM vracks WHERE doc='%i'", doc_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_get_vrackcb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_sql_get failed to execute initial SELECT query"); i_callback_free (cb); return NULL; }

  return cb;
}

int l_vrack_sql_get_vrackcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the SELECT query above */
  i_callback *cb = (i_callback *) passdata;


  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_aciton_sql_list_cb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Check row count */
  int row_count = PQntuples (res);
  if (row_count < 1)
  {
    i_printf (1, "l_aciton_sql_list_cb failed to find vrack");
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  int num;
  int row;
  l_vrack *vrack = l_vrack_create ();
  cb->data = vrack;
  for (row=0; row < row_count; row++)
  {
    /* Fields */
    char *id_str = PQgetvalue (res, row, 0);
    char *ru_count_str = PQgetvalue (res, row, 1) ? : "48";
    char *thumbnail_data = PQgetvalue (res, row, 2);

    /* Setup vrack */
    if (id_str) vrack->doc_id = atoi (id_str);
    if (ru_count_str) vrack->ru_count = atoi (ru_count_str);
    if (thumbnail_data) vrack->thumbnail_data = strdup (thumbnail_data);
  }

  /* Query vrack devices */
  char *query;
  asprintf (&query, "SELECT uuid, ru_index, size, color_red, color_green, color_blue, ent_type, site_name, site_desc, site_suburb, dev_name, dev_desc FROM vrack_devices WHERE doc='%i'", vrack->doc_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_get_devicecb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_sql_get failed to execute vrack_devices SELECT query"); i_callback_free (cb); return -1; }

  return 0;
}

int l_vrack_sql_get_devicecb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the vrack_devices SELECT query */
  i_callback *cb = (i_callback *) passdata;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_vrack_sql_get_devicecb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  int num;
  int row;
  int row_count;
  l_vrack *vrack = cb->data;
  cb->data = vrack;
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *uuid_str;
    char *ru_index_str;
    char *size_str;
    char *red_str;
    char *green_str;
    char *blue_str;
    char *ent_type_str;
    char *site_name;
    char *site_desc;
    char *site_suburb;
    char *dev_name;
    char *dev_desc;
    
    /* Fields */
    uuid_str = PQgetvalue (res, row, 0);
    ru_index_str = PQgetvalue (res, row, 1);
    size_str = PQgetvalue (res, row, 2);
    red_str = PQgetvalue (res, row, 3);
    green_str = PQgetvalue (res, row, 4);
    blue_str = PQgetvalue (res, row, 5);
    ent_type_str = PQgetvalue (res, row, 6);
    site_name = PQgetvalue (res, row, 7);
    site_desc = PQgetvalue (res, row, 8);
    site_suburb = PQgetvalue (res, row, 9);
    dev_name = PQgetvalue (res, row, 10);
    dev_desc = PQgetvalue (res, row, 11);

    /* Create device */
    l_vrack_device *device = l_vrack_device_create ();
    device->uuid_str = strdup (uuid_str);
    device->ru_index = atoi (ru_index_str);
    device->size = atoi (size_str);
    device->color_red = atof (red_str);
    device->color_green = atof (green_str);
    device->color_blue = atof (blue_str);

    /* Create entity */
    if (atoi(ent_type_str) > 0)
    {
      device->entdesc = i_entity_descriptor_create ();
      if (ent_type_str) device->entdesc->type = atoi (ent_type_str);
      device->entdesc->cust_name = strdup (self->hierarchy->cust_name);
      device->entdesc->cust_desc = strdup (self->hierarchy->cust_desc);
      if (site_name) device->entdesc->site_name = strdup (site_name);
      if (site_desc) device->entdesc->site_desc = strdup (site_desc);
      if (site_suburb) device->entdesc->site_suburb = strdup (site_suburb);
      if (dev_name) device->entdesc->dev_name = strdup (dev_name);
      if (dev_desc) device->entdesc->dev_desc = strdup (dev_desc);
    }

    /* Enqueue */
    i_list_enqueue (vrack->device_list, device);
  }

  /* Query vrack device */
  char *query;
  asprintf (&query, "SELECT uuid, group_uuid, vlans, notes, locked, color_red, color_green, color_blue, a_ent_type, a_site_name, a_site_desc, a_site_suburb, a_dev_name, a_dev_desc, a_cnt_name, a_cnt_desc, a_obj_name, a_obj_desc, b_ent_type, b_site_name, b_site_desc, b_site_suburb, b_dev_name, b_dev_desc, b_cnt_name, b_cnt_desc, b_obj_name, b_obj_desc FROM vrack_cables WHERE doc='%i'", vrack->doc_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_get_cablecb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_sql_get failed to execute vrack_devices SELECT query"); i_callback_free (cb); return -1; }

  return 0;
}

int l_vrack_sql_get_cablecb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the vrack_cables SELECT query */
  i_callback *cb = (i_callback *) passdata;
  
  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_vrack_sql_get_cablecb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  int num;
  int row;
  int row_count;
  l_vrack *vrack = cb->data;
  cb->data = vrack;
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    /* Fields */
    char *uuid_str = PQgetvalue (res, row, 0);
    char *group_uuid_str = PQgetvalue (res, row, 1);
    char *vlans_str = PQgetvalue (res, row, 2);
    char *notes_str = PQgetvalue (res, row, 3);
    char *locked_str = PQgetvalue (res, row, 4);
    char *red_str = PQgetvalue (res, row, 5);
    char *green_str = PQgetvalue (res, row, 6);
    char *blue_str = PQgetvalue (res, row, 7);
    char *a_ent_type_str = PQgetvalue (res, row, 8);
    char *a_site_name = PQgetvalue (res, row, 9);
    char *a_site_desc = PQgetvalue (res, row, 10);
    char *a_site_suburb = PQgetvalue (res, row, 11);
    char *a_dev_name = PQgetvalue (res, row, 12);
    char *a_dev_desc = PQgetvalue (res, row, 13);
    char *a_cnt_name = PQgetvalue (res, row, 14);
    char *a_cnt_desc = PQgetvalue (res, row, 15);
    char *a_obj_name = PQgetvalue (res, row, 16);
    char *a_obj_desc = PQgetvalue (res, row, 17);
    char *b_ent_type_str = PQgetvalue (res, row, 18);
    char *b_site_name = PQgetvalue (res, row, 19);
    char *b_site_desc = PQgetvalue (res, row, 20);
    char *b_site_suburb = PQgetvalue (res, row, 21);
    char *b_dev_name = PQgetvalue (res, row, 22);
    char *b_dev_desc = PQgetvalue (res, row, 23);
    char *b_cnt_name = PQgetvalue (res, row, 24);
    char *b_cnt_desc = PQgetvalue (res, row, 25);
    char *b_obj_name = PQgetvalue (res, row, 26);
    char *b_obj_desc = PQgetvalue (res, row, 27);

    /* Create cable */
    l_vrack_cable *cable = l_vrack_cable_create ();
    cable->uuid_str = strdup (uuid_str);
    cable->group_uuid_str = strdup (group_uuid_str);
    cable->vlans_str = strdup (vlans_str);
    cable->notes_str = strdup (notes_str);
    cable->locked = atoi (locked_str);
    cable->color_red = atof (red_str);
    cable->color_green = atof (green_str);
    cable->color_blue = atof (blue_str);

    /* Create entity */
    if (atoi(a_ent_type_str) > 0)
    {
      cable->a_int_entdesc = i_entity_descriptor_create ();
      if (a_ent_type_str) cable->a_int_entdesc->type = atoi (a_ent_type_str);
      cable->a_int_entdesc->cust_name = strdup (self->hierarchy->cust_name);
      cable->a_int_entdesc->cust_desc = strdup (self->hierarchy->cust_desc);
      if (a_site_name) cable->a_int_entdesc->site_name = strdup (a_site_name);
      if (a_site_desc) cable->a_int_entdesc->site_desc = strdup (a_site_desc);
      if (a_site_suburb) cable->a_int_entdesc->site_suburb = strdup (a_site_suburb);
      if (a_dev_name) cable->a_int_entdesc->dev_name = strdup (a_dev_name);
      if (a_dev_desc) cable->a_int_entdesc->dev_desc = strdup (a_dev_desc);
      if (a_cnt_name) cable->a_int_entdesc->cnt_name = strdup (a_cnt_name);
      if (a_cnt_desc) cable->a_int_entdesc->cnt_desc = strdup (a_cnt_desc);
      if (a_obj_name) cable->a_int_entdesc->obj_name = strdup (a_obj_name);
      if (a_obj_desc) cable->a_int_entdesc->obj_desc = strdup (a_obj_desc);
    }
    if (atoi(b_ent_type_str) > 0)
    {
      cable->b_int_entdesc = i_entity_descriptor_create ();
      if (b_ent_type_str) cable->b_int_entdesc->type = atoi (b_ent_type_str);
      cable->b_int_entdesc->cust_name = strdup (self->hierarchy->cust_name);
      cable->b_int_entdesc->cust_desc = strdup (self->hierarchy->cust_desc);
      if (b_site_name) cable->b_int_entdesc->site_name = strdup (b_site_name);
      if (b_site_desc) cable->b_int_entdesc->site_desc = strdup (b_site_desc);
      if (b_site_suburb) cable->b_int_entdesc->site_suburb = strdup (b_site_suburb);
      if (b_dev_name) cable->b_int_entdesc->dev_name = strdup (b_dev_name);
      if (b_dev_desc) cable->b_int_entdesc->dev_desc = strdup (b_dev_desc);
      if (b_cnt_name) cable->b_int_entdesc->cnt_name = strdup (b_cnt_name);
      if (b_cnt_desc) cable->b_int_entdesc->cnt_desc = strdup (b_cnt_desc);
      if (b_obj_name) cable->b_int_entdesc->obj_name = strdup (b_obj_name);
      if (b_obj_desc) cable->b_int_entdesc->obj_desc = strdup (b_obj_desc);
    }

    /* Enqueue */
    i_list_enqueue (vrack->cable_list, cable);
  }

  /* Query vrack cablegroups */
  char *query;
  asprintf (&query, "SELECT uuid, descr FROM vrack_cablegroups WHERE doc='%i'", vrack->doc_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_get_cablegroupcb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_sql_get failed to execute vrack_cablegroups SELECT query"); i_callback_free (cb); return -1; }

  return 0;
}

/* Scene SQL Operations */
int l_vrack_sql_get_cablegroupcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the vrack_devices SELECT query */
  i_callback *cb = (i_callback *) passdata;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_vrack_sql_get_cablegroupcb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  int num;
  int row;
  int row_count;
  l_vrack *vrack = cb->data;
  cb->data = vrack;
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    /* Fields */
    char *uuid_str = PQgetvalue (res, row, 0);
    char *desc_str = PQgetvalue (res, row, 1);

    /* Create cablegroup */
    l_vrack_cablegroup *cablegroup = l_vrack_cablegroup_create ();
    cablegroup->uuid_str = strdup (uuid_str);
    cablegroup->desc_str = strdup (desc_str);

    /* Enqueue */
    i_list_enqueue (vrack->cablegroup_list, cablegroup);
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, vrack, cb->passdata);
    if (num != 0) l_vrack_free (vrack);
  }
  else l_vrack_free (vrack);

  /* Cleanup */
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Scene SQL Operations */

int l_vrack_sql_insert (i_resource *self, l_vrack *vrack)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_vrack_sql_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "INSERT INTO vracks (doc, ru_count, thumbnail) VALUES ('%i', '%i', '%s');",
    vrack->doc_id, vrack->ru_count, vrack->thumbnail_data);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_sql_insert failed to execute INSERT for new vrack"); return -1; }

  return 0;
}

int l_vrack_sql_delete (i_resource *self, int doc_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_vrack_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM vracks WHERE doc='%i'", doc_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_sql_update failed to execute DELETE for vrack %li", doc_id); return -1; }

  return 0;
}

/* VRack Device SQL Operations */

int l_vrack_sql_device_insert (i_resource *self, l_vrack *vrack, l_vrack_device *device)
{
  int num;
  char *site_name;
  char *site_desc;
  char *site_suburb;
  char *dev_name;
  char *dev_desc;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_vrack_sql_device_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  if (device->entdesc && device->entdesc->site_name) 
  { asprintf (&site_name, "'%s'",  device->entdesc->site_name); asprintf (&site_desc, "'%s'", device->entdesc->site_desc); }
  else 
  { asprintf (&site_name, "NULL"); asprintf (&site_desc, "NULL"); }
  if (device->entdesc && device->entdesc->site_suburb) 
  { asprintf (&site_suburb, "'%s'", device->entdesc->site_suburb); }
  else 
  { asprintf (&site_suburb, "NULL"); }
  if (device->entdesc && device->entdesc->dev_name) 
  { asprintf (&dev_name, "'%s'",  device->entdesc->dev_name); asprintf (&dev_desc, "'%s'", device->entdesc->dev_desc); }
  else 
  { asprintf (&dev_name, "NULL"); asprintf (&dev_desc, "NULL"); }
  
  asprintf (&query, "INSERT INTO vrack_devices (doc, uuid, ru_index, size, color_red, color_green, color_blue, ent_type, site_name, site_desc, site_suburb, dev_name, dev_desc) VALUES ('%li', '%s', '%i', '%i', '%.2f', '%.2f', '%.2f', '%i', %s, %s, %s, %s, %s);",
    vrack->doc_id, device->uuid_str, device->ru_index, device->size, 
    device->color_red, device->color_green, device->color_blue, 
    device->entdesc ? device->entdesc->type : 0, site_name, site_desc, site_suburb, dev_name, dev_desc);
  if (site_name) free (site_name);
  if (site_desc) free (site_desc);
  if (site_suburb) free (site_suburb);
  if (dev_name) free (dev_name);
  if (dev_desc) free (dev_desc);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_sql_device_insert failed to execute INSERT for vrack device for document %li", vrack->doc_id); return -1; }

  return 0;
}

int l_vrack_sql_device_delete (i_resource *self, long doc_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_vrack_device_sql_delete failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM vrack_devices WHERE doc='%i'", doc_id);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_device_sql_delete failed to execute delete for doc_id %li", doc_id); return -1; }

  return 0;
}

int l_vrack_sql_device_delete_device (i_resource *self, i_device *dev)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_vrack_sql_device_delete_device failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM vrack_devices WHERE site_name='%s' AND dev_name='%s'", dev->site->name_str, dev->name_str);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_sql_device_delete_device failed to execute delete"); return -1; }

  return 0;
}

/* VRack Device SQL Operations */

int l_vrack_sql_cable_insert (i_resource *self, l_vrack *vrack, l_vrack_cable *cable)
{
  int num;
  char *a_site_name;
  char *a_site_desc;
  char *a_site_suburb;
  char *a_dev_name;
  char *a_dev_desc;
  char *a_cnt_name;
  char *a_cnt_desc;
  char *a_obj_name;
  char *a_obj_desc;
  char *b_site_name;
  char *b_site_desc;
  char *b_site_suburb;
  char *b_dev_name;
  char *b_dev_desc;
  char *b_cnt_name;
  char *b_cnt_desc;
  char *b_obj_name;
  char *b_obj_desc;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_vrack_sql_device_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  if (cable->a_int_entdesc && cable->a_int_entdesc->site_name)
  { asprintf (&a_site_name, "'%s'",  cable->a_int_entdesc->site_name); asprintf (&a_site_desc, "'%s'", cable->a_int_entdesc->site_desc); }
  else
  { asprintf (&a_site_name, "NULL"); asprintf (&a_site_desc, "NULL"); }
  if (cable->a_int_entdesc && cable->a_int_entdesc->site_suburb)
  { asprintf (&a_site_suburb, "'%s'", cable->a_int_entdesc->site_suburb); }
  else
  { asprintf (&a_site_suburb, "NULL"); }
  if (cable->a_int_entdesc && cable->a_int_entdesc->dev_name)
  { asprintf (&a_dev_name, "'%s'",  cable->a_int_entdesc->dev_name); asprintf (&a_dev_desc, "'%s'", cable->a_int_entdesc->dev_desc); }
  else
  { asprintf (&a_dev_name, "NULL"); asprintf (&a_dev_desc, "NULL"); }
  if (cable->a_int_entdesc && cable->a_int_entdesc->cnt_name)
  { asprintf (&a_cnt_name, "'%s'",  cable->a_int_entdesc->cnt_name); asprintf (&a_cnt_desc, "'%s'", cable->a_int_entdesc->cnt_desc); }
  else
  { asprintf (&a_cnt_name, "NULL"); asprintf (&a_cnt_desc, "NULL"); }
  if (cable->a_int_entdesc && cable->a_int_entdesc->obj_name)
  { asprintf (&a_obj_name, "'%s'",  cable->a_int_entdesc->obj_name); asprintf (&a_obj_desc, "'%s'", cable->a_int_entdesc->obj_desc); }
  else
  { asprintf (&a_obj_name, "NULL"); asprintf (&a_obj_desc, "NULL"); }
  if (cable->b_int_entdesc && cable->b_int_entdesc->site_name)
  { asprintf (&b_site_name, "'%s'",  cable->b_int_entdesc->site_name); asprintf (&b_site_desc, "'%s'", cable->b_int_entdesc->site_desc); }
  else
  { asprintf (&b_site_name, "NULL"); asprintf (&b_site_desc, "NULL"); }
  if (cable->b_int_entdesc && cable->b_int_entdesc->site_suburb)
  { asprintf (&b_site_suburb, "'%s'", cable->b_int_entdesc->site_suburb); }
  else
  { asprintf (&b_site_suburb, "NULL"); }
  if (cable->b_int_entdesc && cable->b_int_entdesc->dev_name)
  { asprintf (&b_dev_name, "'%s'",  cable->b_int_entdesc->dev_name); asprintf (&b_dev_desc, "'%s'", cable->b_int_entdesc->dev_desc); }
  else
  { asprintf (&b_dev_name, "NULL"); asprintf (&b_dev_desc, "NULL"); }
  if (cable->b_int_entdesc && cable->b_int_entdesc->cnt_name)
  { asprintf (&b_cnt_name, "'%s'",  cable->b_int_entdesc->cnt_name); asprintf (&b_cnt_desc, "'%s'", cable->b_int_entdesc->cnt_desc); }
  else
  { asprintf (&b_cnt_name, "NULL"); asprintf (&b_cnt_desc, "NULL"); }
  if (cable->b_int_entdesc && cable->b_int_entdesc->obj_name)
  { asprintf (&b_obj_name, "'%s'",  cable->b_int_entdesc->obj_name); asprintf (&b_obj_desc, "'%s'", cable->b_int_entdesc->obj_desc); }
  else
  { asprintf (&b_obj_name, "NULL"); asprintf (&b_obj_desc, "NULL"); }
  asprintf (&query, "INSERT INTO vrack_cables (doc, uuid, group_uuid, vlans, notes, locked, color_red, color_green, color_blue, a_ent_type, a_site_name, a_site_desc, a_site_suburb, a_dev_name, a_dev_desc, a_cnt_name, a_cnt_desc, a_obj_name, a_obj_desc, b_ent_type, b_site_name, b_site_desc, b_site_suburb, b_dev_name, b_dev_desc, b_cnt_name, b_cnt_desc, b_obj_name, b_obj_desc) VALUES ('%li', '%s', '%s', '%s', '%s', '%i', '%.2f', '%.2f', '%.2f', '%i', %s, %s, %s, %s, %s, %s, %s, %s, %s, '%i', %s, %s, %s, %s, %s, %s, %s, %s, %s);",
    vrack->doc_id, cable->uuid_str, cable->group_uuid_str, cable->vlans_str, cable->notes_str, cable->locked,
    cable->color_red, cable->color_green, cable->color_blue, 
    cable->a_int_entdesc ? cable->a_int_entdesc->type : 0, 
    a_site_name, a_site_desc, a_site_suburb, a_dev_name, a_dev_desc, a_cnt_name, a_cnt_desc, a_obj_name, a_obj_desc,
    cable->b_int_entdesc ? cable->b_int_entdesc->type : 0, 
    b_site_name, b_site_desc, b_site_suburb, b_dev_name, b_dev_desc, b_cnt_name, b_cnt_desc, b_obj_name, b_obj_desc);
  if (a_site_name) free (a_site_name);
  if (a_site_desc) free (a_site_desc);
  if (a_site_suburb) free (a_site_suburb);
  if (a_dev_name) free (a_dev_name);
  if (a_dev_desc) free (a_dev_desc);
  if (a_cnt_name) free (a_cnt_name);
  if (a_cnt_desc) free (a_cnt_desc);
  if (a_obj_name) free (a_obj_name);
  if (a_obj_desc) free (a_obj_desc);
  if (b_site_name) free (b_site_name);
  if (b_site_desc) free (b_site_desc);
  if (b_site_suburb) free (b_site_suburb);
  if (b_dev_name) free (b_dev_name);
  if (b_dev_desc) free (b_dev_desc);
  if (b_cnt_name) free (b_cnt_name);
  if (b_cnt_desc) free (b_cnt_desc);
  if (b_obj_name) free (b_obj_name);
  if (b_obj_desc) free (b_obj_desc);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_cable_sql_insert failed to execute INSERT for vrack cable for document %li", vrack->doc_id); return -1; }

  return 0;
}

int l_vrack_sql_cable_delete (i_resource *self, long doc_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_vrack_cable_sql_delete failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM vrack_cables WHERE doc='%i'", doc_id);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_cable_sql_delete failed to execute delete for doc_id %li", doc_id); return -1; }

  return 0;
}

int l_vrack_sql_cable_delete_device (i_resource *self, i_device *dev)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_vrack_sql_cable_delete_device failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM vrack_cables WHERE (a_site_name='%s' AND a_dev_name='%s') OR (b_site_name='%s' AND b_dev_name='%s')", dev->site->name_str, dev->name_str, dev->site->name_str, dev->name_str);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_sql_cable_delete_device failed to execute delete"); return -1; }

  return 0;
}

/* VRack Device SQL Operations */

int l_vrack_sql_cablegroup_insert (i_resource *self, l_vrack *vrack, l_vrack_cablegroup *cablegroup)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_vrack_sql_cablegroup_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "INSERT INTO vrack_cablegroups (doc, uuid, descr) VALUES ('%li', '%s', '%s');",
    vrack->doc_id, cablegroup->uuid_str, cablegroup->desc_str);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_sql_cablegroup_insert failed to execute INSERT for vrack cablegroup for document %li", vrack->doc_id); return -1; }

  return 0;
}

int l_vrack_sql_cablegroup_delete (i_resource *self, long doc_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_vrack_cablegroup_sql_delete failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM vrack_cablegroups WHERE doc='%i'", doc_id);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_vrack_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_vrack_cablegroup_sql_delete failed to execute delete for doc_id %li", doc_id); return -1; }

  return 0;
}

/* SQL Callback */

int l_vrack_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_vrack_sql_cb failed to execute query (%s)", PQresultErrorMessage (result)); return -1; }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

