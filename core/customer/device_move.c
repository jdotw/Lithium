#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/inventory.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/entity_xmlsync.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>
#include <induction/timer.h>

#include "device.h"
#include "lic.h"

/* Device Related Functions - Move  
 *
 * Moves the devices to the specified destination site. 
 * This process will:
 *
 * 1) Update devices table and live device list
 * 2) Move the device file system directory
 * 3) Update related SQL tables (incidents, services, etc)
 * 4) Restart the device monitoring process
 *
 * All SQL operations are done synchronously.
 * The file system move uses a rename command will NOT work across
 * filesystems.
 *
 */

int l_device_move (i_resource *self, i_device *dev, i_site *dest_site)
{
  /* 
   * Prepare for the move
   */

  /* Open postgresql session */
  PGconn *pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_device_move failed to connect to SQL database"); return -1; }

  /* Get current fsroot */
  char *old_fsroot = i_entity_path (self, ENTITY(dev), 0, ENTPATH_ROOT);

  /* Get current site name */
  char *old_site = strdup (dev->site->name_str);

  /* 
   * Update live device
   */

  /* De-register at old site */
  i_entity_deregister (self, ENTITY(dev));

  /* Register to new site */
  i_entity_register (self, ENTITY(dest_site), ENTITY(dev));

  /* 
   * Move filesystem
   */

  /* Rename file system */
  char *new_fsroot = i_entity_path (self, ENTITY(dev), 0, ENTPATH_ROOT);
  int num = rename (old_fsroot, new_fsroot);
  if (num != 0)
  { i_printf (1, "l_device_move warning, failed to move %s to %s", old_fsroot, new_fsroot); } 
  free (new_fsroot);
  free (old_fsroot);

  /* 
   * Update SQL Tables
   */

  char *query;
  PGresult *result;

  /* Update device SQL table */
  asprintf (&query, "UPDATE devices SET site='%s' WHERE site='%s' AND name='%s'", dest_site->name_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update devices SQL table"); }
  if (result) PQclear (result);

  /* Update groups SQL table */
  asprintf (&query, "UPDATE group_entities SET site_name='%s', site_desc='%s' WHERE site_name='%s' AND dev_name='%s'", dest_site->name_str, dest_site->desc_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update group_entities SQL table"); }
  if (result) PQclear (result);

  /* Update old inventory_static_items SQL table */
  asprintf (&query, "UPDATE inventory_static_items SET site_id='%s' WHERE site_id='%s' AND device_id='%s'", dest_site->name_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update inventory_static_items SQL table"); }
  if (result) PQclear (result);

  /* Update old inventory_item_history SQL table */
  asprintf (&query, "UPDATE inventory_item_history SET site_id='%s' WHERE site_id='%s' AND device_id='%s'", dest_site->name_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update inventory_item_history SQL table"); }
  if (result) PQclear (result);

  /* Update Services SQL table */
  asprintf (&query, "UPDATE services SET site='%s' WHERE site='%s' AND device='%s'", dest_site->name_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update services SQL table"); }
  if (result) PQclear (result);

  /* Update Incidents SQL table */
  asprintf (&query, "UPDATE incidents SET site_name='%s', site_desc='%s', site_suburb='%s' WHERE site_name='%s' AND dev_name='%s'", dest_site->name_str, dest_site->desc_str, dest_site->suburb_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update incidents SQL table"); }
  if (result) PQclear (result);

  /* Update Case Entities SQL table */
  asprintf (&query, "UPDATE case_entities SET site_name='%s', site_desc='%s', site_suburb='%s' WHERE site_name='%s' AND dev_name='%s'", dest_site->name_str, dest_site->desc_str, dest_site->suburb_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update case entities SQL table"); }
  if (result) PQclear (result);

  /* Update Action Entities SQL table */
  asprintf (&query, "UPDATE action_entities SET site_name='%s', site_desc='%s', site_suburb='%s' WHERE site_name='%s' AND dev_name='%s'", dest_site->name_str, dest_site->desc_str, dest_site->suburb_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update action entities SQL table"); }
  if (result) PQclear (result);

  /* Update Action History SQL table */
  asprintf (&query, "UPDATE action_history SET site_name='%s', site_desc='%s', site_suburb='%s' WHERE site_name='%s' AND dev_name='%s'", dest_site->name_str, dest_site->desc_str, dest_site->suburb_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update action history SQL table"); }
  if (result) PQclear (result);

  /* Update Record Rules SQL table */
  asprintf (&query, "UPDATE record_recrules SET site_name='%s', site_desc='%s' WHERE site_name='%s' AND dev_name='%s'", dest_site->name_str, dest_site->desc_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update record rules SQL table"); }
  if (result) PQclear (result);

  /* Update Triggerset Apprules table */
  asprintf (&query, "UPDATE triggerset_apprules SET site_name='%s', site_desc='%s' WHERE site_name='%s' AND dev_name='%s'", dest_site->name_str, dest_site->desc_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update triggerset app rules SQL table"); }
  if (result) PQclear (result);

  /* Update Triggerset Valrules table */
  asprintf (&query, "UPDATE triggerset_valrules SET site_name='%s', site_desc='%s' WHERE site_name='%s' AND dev_name='%s'", dest_site->name_str, dest_site->desc_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update triggerset value rules SQL table"); }
  if (result) PQclear (result);

  /* Update VRack Devices table */
  asprintf (&query, "UPDATE vrack_devices SET site_name='%s', site_desc='%s' WHERE site_name='%s' AND dev_name='%s'", dest_site->name_str, dest_site->desc_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update vrack devices SQL table"); }
  if (result) PQclear (result);

  /* Update VRack Cables table */
  asprintf (&query, "UPDATE triggerset_valrules SET a_site_name='%s', a_site_desc='%s' WHERE a_site_name='%s' AND a_dev_name='%s'", dest_site->name_str, dest_site->desc_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update vrack cable a-end SQL table"); }
  if (result) PQclear (result);
  asprintf (&query, "UPDATE triggerset_valrules SET b_site_name='%s', b_site_desc='%s' WHERE b_site_name='%s' AND b_dev_name='%s'", dest_site->name_str, dest_site->desc_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update vrack cable a-end SQL table"); }
  if (result) PQclear (result);

  /* Update Scene overlays table */
  asprintf (&query, "UPDATE scene_overlays SET site_name='%s', site_desc='%s' WHERE site_name='%s' AND dev_name='%s'", dest_site->name_str, dest_site->desc_str, old_site, dev->name_str);
  result = PQexec (pgconn, query);
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_move warning, failed to update scene overlays SQL table"); }
  if (result) PQclear (result);

  /* Close PostgreSQL conn */
  i_pg_close (pgconn);

  /*
   * Restart Device
   */

  /* License */
  dev->licensed = l_lic_take (self, ENTITY(dev));

  /* Restart device resource */
  num = l_device_res_restart (self, dev);
  if (num != 0)
  { i_printf (1, "l_device_move warning, failed to restart device resource for %s at %s", dev->name_str, dev->site->name_str); }

  return 0;
}

