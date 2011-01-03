#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "induction.h"
#include "list.h"
#include "construct.h"
#include "cement.h"
#include "entity.h"
#include "customer.h"
#include "site.h"
#include "device.h"
#include "container.h"
#include "object.h"
#include "hierarchy.h"
#include "metric.h"
#include "postgresql.h"
#include "files.h"
#include "configfile.h"

#include "search.h"

/* 
 * Search Cache Functions
 *
 * A table in the customer database is used to 
 * store a cache of entity descriptions with the
 * entity address and resource address for the entity
 *
 * These functions are the non-specific functions
 * for inserting/removing rows from the search
 * cache. 
 *
 * The insert/delete of individual rows is done during
 * entity registration. The customer and device modules 
 * take care of table scrubbing. 
 */

/* Cache Insert/Delete */

int i_searchcache_insert (i_resource *self, i_entity *entity)
{
  i_debug("i_searchcache_insert called to do insert of %i:%s", entity->ent_type, entity->desc_str);

  /* Check to see if deep search is enabled */
  if (self->type == RES_DEVICE)
  {
    char *conf_str = i_configfile_get (self, NODECONF_FILE, "search", "deep", 0);
    if (!conf_str || atoi(conf_str) != 1) return 0;     // Disable deep search
  }

  /* Set dev_name string or NULL */
  char *devname_str;
  if (self->hierarchy->dev)
  { asprintf(&devname_str, "'%s'", self->hierarchy->dev->name_str); }
  else 
  { devname_str = strdup ("NULL"); }

  /* Set desc strings or NULL */
  i_entity_descriptor *entdesc = i_entity_descriptor_struct_static(entity);
  char *cust_desc;
  if (entdesc->cust_desc) asprintf(&cust_desc, "'%s'", entdesc->cust_desc);
  else cust_desc = strdup("NULL"); 
  char *site_desc;
  if (entdesc->site_desc) asprintf(&site_desc, "'%s'", entdesc->site_desc);
  else site_desc = strdup("NULL"); 
  char *dev_desc;
  if (entdesc->dev_desc) asprintf(&dev_desc, "'%s'", entdesc->dev_desc);
  else dev_desc = strdup("NULL"); 
  char *cnt_desc;
  if (entdesc->cnt_desc) asprintf(&cnt_desc, "'%s'", entdesc->cnt_desc);
  else cnt_desc = strdup("NULL"); 
  char *obj_desc;
  if (entdesc->obj_desc) asprintf(&obj_desc, "'%s'", entdesc->obj_desc);
  else obj_desc = strdup("NULL"); 

  /* Create query */
  char *entaddr_str = i_entity_address_string (entity, NULL);
  char *resaddr_str = i_resource_address_struct_to_string(RES_ADDR(self));
  char *desc_str = entity->desc_str ? : entity->name_str;
  char *insert_str;
  asprintf (&insert_str, "INSERT INTO searchcache (ent_type, descr, ent_address, res_address, dev_name, cust_desc, site_desc, dev_desc, cnt_desc, obj_desc) VALUES (%i, '%s', '%s', '%s', %s, %s, %s, %s, %s, %s)",
    entity->ent_type, desc_str, entaddr_str, resaddr_str, devname_str, cust_desc, site_desc, dev_desc, cnt_desc, obj_desc);
  free (entaddr_str);
  free (resaddr_str);
  free (devname_str);
  free (cust_desc);
  free (site_desc);
  free (dev_desc);
  free (cnt_desc);
  free (obj_desc);

  /* Connect to SQL db (sync) */
  PGconn *pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "i_searchcache_insert failed to connect to postgres database"); return -1; }

  /* Check the table exists and create it if necessary */
  PGresult *result = PQexec (pgconn, insert_str);
  free(insert_str);
  if (result) { PQclear(result); result = NULL; }

  /* Close DB */
  i_pg_close (pgconn);

  return 0;
}

int i_searchcache_delete (i_resource *self, i_entity *entity)
{
  /* Create query */
  char *entaddr_str = i_entity_address_string (entity, NULL);
  char *delete_str;
  asprintf (&delete_str, "DELETE FROM searchcache WHERE address='%s'", entaddr_str);
  free (entaddr_str);

  /* Connect to SQL db (sync) */
  PGconn *pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "i_searchcache_delete failed to connect to postgres database"); return -1; }

  /* Check the table exists and create it if necessary */
  PGresult *result = PQexec (pgconn, delete_str);
  free (delete_str);
  if (result) { PQclear(result); result = NULL; }

  /* Close DB */
  i_pg_close (pgconn);

  return 0;
}


