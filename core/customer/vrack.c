#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>
#include <induction/xml.h>

#include "vrack.h"

/*
 * VRack - User define wiring cloest rack layouts
 *
 *         Uses l_document for document storage.
 */

/* Enable Sub-System */

int l_vrack_enable (i_resource *self)
{
  PGconn *pgconn;
  PGresult *pgres;

  /* Connect to SQL */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_vrack_enable failed to connect to postgres database"); return -1; }

  /* Check vracks table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'vracks' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    if (pgres) { PQclear (pgres); pgres = NULL; }
    pgres = PQexec (pgconn, "CREATE TABLE vracks (doc integer, ru_count integer, thumbnail bytea)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_vrack_enable failed to create vracks table (%s)", PQresultErrorMessage (pgres)); }
  }
  if (pgres) { PQclear (pgres); pgres = NULL; }

  /* Check vracks table has thumbnail column */
  pgres = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='vracks' AND column_name='thumbnail' ORDER BY ordinal_position");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    if (pgres) { PQclear(pgres); pgres = NULL; }
    i_printf (0, "l_vrack_enable version-specific check: 'thumbnail' column missing, attempting to add it");
    pgres = PQexec (pgconn, "ALTER TABLE vracks ADD COLUMN thumbnail bytea");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_vrack_enable failed to add thumbnail column (%s)", PQresultErrorMessage (pgres)); }
  }
  if (pgres) { PQclear(pgres); pgres = NULL; }

  /* Check vrack_devices table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'vrack_devices' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    if (pgres) { PQclear (pgres); pgres = NULL; }
    pgres = PQexec (pgconn, "CREATE TABLE vrack_devices (doc integer, uuid varchar, ru_index integer, size integer, color_red real, color_green real, color_blue real, ent_type integer, site_name varchar, site_desc varchar, site_suburb varchar, dev_name varchar, dev_desc varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_vrack_enable failed to create devices table (%s)", PQresultErrorMessage (pgres)); }
  }
  if (pgres) { PQclear (pgres); pgres = NULL; }

  /* Check vrack_cables table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'vrack_cables' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    if (pgres) { PQclear (pgres); pgres = NULL; }
    pgres = PQexec (pgconn, "CREATE TABLE vrack_cables (doc integer, uuid varchar, group_uuid varchar, vlans varchar, notes varchar, locked integer, color_red real, color_green real, color_blue real, a_ent_type integer, a_site_name varchar, a_site_desc varchar, a_site_suburb varchar, a_dev_name varchar, a_dev_desc varchar, a_cnt_name varchar, a_cnt_desc varchar, a_obj_name varchar, a_obj_desc varchar, b_ent_type integer, b_site_name varchar, b_site_desc varchar, b_site_suburb varchar, b_dev_name varchar, b_dev_desc varchar, b_cnt_name varchar, b_cnt_desc varchar, b_obj_name varchar, b_obj_desc varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_vrack_enable failed to create cables table (%s)", PQresultErrorMessage (pgres)); }
  }
  if (pgres) { PQclear (pgres); pgres = NULL; }

  /* Check vrack_cablegroups table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'vrack_cablegroups' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    if (pgres) { PQclear (pgres); pgres = NULL; }
    pgres = PQexec (pgconn, "CREATE TABLE vrack_cablegroups (doc integer, uuid varchar, descr varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_vrack_enable failed to create cablegroups table (%s)", PQresultErrorMessage (pgres)); }
  }
  if (pgres) { PQclear (pgres); pgres = NULL; }

  /* Close SQL */
  i_pg_close (pgconn);

  return 0;
}

/* Structs */

l_vrack* l_vrack_create ()
{
  l_vrack *vrack = malloc (sizeof(l_vrack));
  memset (vrack, 0, sizeof(l_vrack));

  vrack->device_list = i_list_create ();
  i_list_set_destructor (vrack->device_list, l_vrack_device_free);
  vrack->cable_list = i_list_create ();
  i_list_set_destructor (vrack->cable_list, l_vrack_cable_free);
  vrack->cablegroup_list = i_list_create ();
  i_list_set_destructor (vrack->cablegroup_list, l_vrack_cablegroup_free);

  vrack->ru_count = 48;

  return vrack;
}

void l_vrack_free (void *vrackptr)
{
  l_vrack *vrack = vrackptr;

  if (vrack->thumbnail_data) free (vrack->thumbnail_data);
  if (vrack->device_list) i_list_free (vrack->device_list);
  if (vrack->cable_list) i_list_free (vrack->cable_list);
  if (vrack->cablegroup_list) i_list_free (vrack->cablegroup_list);

  free (vrack);
}

l_vrack_device* l_vrack_device_create ()
{
  l_vrack_device *vrack_device = malloc (sizeof(l_vrack_device));
  memset (vrack_device, 0, sizeof(l_vrack_device));

  return vrack_device;
}

void l_vrack_device_free (void *vrack_deviceptr)
{
  l_vrack_device *vrack_device = vrack_deviceptr;

  if (vrack_device->entdesc) i_entity_descriptor_free (vrack_device->entdesc);

  free (vrack_device);
}

l_vrack_cable* l_vrack_cable_create ()
{
  l_vrack_cable *vrack_cable = malloc (sizeof(l_vrack_cable));
  memset (vrack_cable, 0, sizeof(l_vrack_cable));

  return vrack_cable;
}

void l_vrack_cable_free (void *vrack_cableptr)
{
  l_vrack_cable *vrack_cable = vrack_cableptr;

  if (vrack_cable->a_int_entdesc) i_entity_descriptor_free (vrack_cable->a_int_entdesc);
  if (vrack_cable->b_int_entdesc) i_entity_descriptor_free (vrack_cable->b_int_entdesc);
  if (vrack_cable->vlans_str) free (vrack_cable->vlans_str);
  if (vrack_cable->notes_str) free (vrack_cable->notes_str);

  free (vrack_cable);
}

l_vrack_cablegroup* l_vrack_cablegroup_create ()
{
  l_vrack_cablegroup *vrack_cablegroup = malloc (sizeof(l_vrack_cablegroup));
  memset (vrack_cablegroup, 0, sizeof(l_vrack_cablegroup));

  return vrack_cablegroup;
}

void l_vrack_cablegroup_free (void *vrack_cablegroupptr)
{
  l_vrack_cablegroup *vrack_cablegroup = vrack_cablegroupptr;

  if (vrack_cablegroup->desc_str) free (vrack_cablegroup->desc_str);

  free (vrack_cablegroup);
}




