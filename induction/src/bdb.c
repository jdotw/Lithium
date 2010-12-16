#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "induction.h"
#include "bdb.h"
#include "path.h"
#include "list.h"

/* i_db struct manipulation */

void i_db_free (i_db *db)
{
  if (!db) return;

  if (db->file) free (db->file);
  if (db->db) db->db->close (db->db, 0); 

  free (db);
}

/* Open (and create) */ 

i_db* i_db_open (i_resource *self, char *dbname)
{
  int num; 
  i_db *db;

  db = (i_db *) malloc (sizeof(i_db));
  if (!db)
  { i_printf (1, "i_db_open failed to malloc db"); return NULL; }
  memset (db, 0, sizeof(i_db));
  
  db->file = i_path_glue (self->root, dbname);
  if (!db->file)
  { 
    i_printf (1, "i_db_open failed to create db->file");
    return NULL;
  }
    
  num = db_create (&db->db, NULL, 0);
  if (num != 0)
  {
    i_printf (1, "i_db_open failed to call db_create (%s)", db_strerror(num));
    i_db_free (db);
    return NULL;
  }

  num = db->db->set_flags (db->db, DB_DUP);
  if (num != 0)
  {
    i_printf (1, "i_db_open failed to set DB_DUP flag (%s)", db_strerror(num));
    i_db_free (db);
    return NULL;
  }

  num = db->db->open (db->db, NULL, db->file, NULL, DB_BTREE, DB_CREATE, 0664);
  if (num != 0) 
  {
    i_printf (1, "i_db_open failed to open db %s (%s)", db->file, db_strerror(num));
    i_db_free (db);
    return NULL;
  }

  return db;
}

/* Record Struct Manipulation */

void i_db_free_record (void *recordptr)
{
  i_db_record *record = recordptr;

  if (!record) return;
  if (record->key) free (record->key);
  if (record->data) free (record->data);
  free (record);
}

i_db_record* i_db_record_create (void *key, int keysize, void *data, int datasize)
{
  i_db_record *record;

  record = (i_db_record *) malloc (sizeof(i_db_record));
  if (!record)
  { i_printf (1, "i_db_record_create failed to malloc record"); return NULL; }
  memset (record, 0, sizeof(i_db_record));

  if (key && keysize > 0)
  {
    record->key = malloc (keysize);
    if (!record->key)
    {
      i_printf (1, "i_db_record_create failed to malloc record->key (%i bytes)", keysize);
      i_db_free_record (record);
      return NULL;
    }
    memcpy (record->key, key, keysize);
    record->keysize = keysize;
  }
  if (data && datasize > 0)
  {
    record->data = malloc (datasize);
    if (!record->data)
    { 
      i_printf (1, "i_db_record_create failed to malloc record->data (%i bytes)", datasize); 
      i_db_free_record (record);
      return NULL; 
    }
    memcpy (record->data, data, datasize);
    record->datasize = datasize;
  }
  
  return record;
}

/* Put */

int i_db_put (i_resource *self, i_db *db, void *key, int key_size, void *data, int data_size)
{
  int num;
  DBT dbkey;
  DBT dbdata;
  DBC *cursor;

  memset (&dbkey, 0, sizeof(DBT));
  memset (&dbdata, 0, sizeof(DBT));

  dbkey.data = key;
  dbkey.size = key_size;
  dbdata.data = data;
  dbdata.size = data_size;

  num = db->db->cursor (db->db, NULL, &cursor, 0);
  if (num != 0)
  {
    i_printf (1, "i_db_put failed to get db cursor (%s)", db_strerror(num));
    return -1;
  }      
  
  num = cursor->c_put (cursor, &dbkey, &dbdata, DB_KEYLAST);
  cursor->c_close (cursor);
  if (num != 0)
  {
    i_printf (1, "i_db_put failed to put record (%s)", db_strerror(num));
    return -1;
  }

  return 0;
}

/* Get */

i_db_record* i_db_get (i_resource *self, i_db *db, void *key, int key_size)
{
  int num;
  DBT dbdata;
  DBT dbkey;
  DBC *cursor;
  i_db_record *record;
  
  memset (&dbkey, 0, sizeof(DBT));              /* Clear the key DBT for writing */
  memset (&dbdata, 0, sizeof(DBT));             /* Clear the data DBT for writing */

  dbkey.data = key;
  dbkey.size = key_size;

  num = db->db->cursor (db->db, NULL, &cursor, 0);
  if (num != 0)
  {
    i_printf (1, "i_db_get failed to get db cursor (%s)", db_strerror(num));
    return NULL;
  }            
  
  num = cursor->c_get (cursor, &dbkey, &dbdata, DB_SET);
  cursor->c_close (cursor);
  if (num != 0)
  { 
    i_printf (2, "i_db_get failed to get record (%s) (this may not be a fault as such)", db_strerror(num));
    return NULL;
  }

  record = i_db_record_create (key, key_size, dbdata.data, dbdata.size);
  if (!record)
  {
    i_printf (1, "i_db_get failed to create return record");
    return NULL;
  }

  return record;
}

i_list* i_db_get_all_list (i_resource *self, i_db *db)    /* Get all records from the db as a list of records */
{
  int num;
  DBT dbkey;
  DBT dbdata;
  DBC *cursor;
  i_list *list;

  list = i_list_create ();
  if (!list)
  {
    i_printf (1, "i_db_get_all_list failed to create list");
    return NULL;
  }
  i_list_set_destructor (list, i_db_free_record);
  
  memset (&dbkey, 0, sizeof(dbkey));
  memset (&dbdata, 0, sizeof(dbdata));

  num = db->db->cursor (db->db, NULL, &cursor, 0);
  if (num != 0)
  {
    i_printf (1, "i_db_get_all_list failed to get db cursor (%s)", db_strerror(num));
    i_list_free (list);
    return NULL;
  }                  

  while ((num = cursor->c_get (cursor, &dbkey, &dbdata, DB_NEXT)) == 0)
  {
    /* Loop for each item in the DB */
    i_db_record *record;

    record = i_db_record_create (dbkey.data, dbkey.size, dbdata.data, dbdata.size);
    if (!record)
    {
      i_printf (1, "i_db_get_all_list failed to create record, continuing");
      continue;
    }

    i_list_enqueue (list, record);
  }
  cursor->c_close (cursor);

  return list;
}

/* Del */

int i_db_del (i_resource *self, i_db *db, void *key, int key_size)
{
  int num;
  DBT dbkey;
  DBT dbdata;
  DBC *cursor;

  memset (&dbkey, 0, sizeof(DBT));
  memset (&dbdata, 0, sizeof(DBT));
  dbkey.data = key;
  dbkey.size = key_size;
  
  /* First set the cursor to the right record */

  num = db->db->cursor (db->db, NULL, &cursor, 0);
  if (num != 0)
  {
    i_printf (1, "i_db_del failed to get db cursor (%s)", db_strerror(num));
    return -1;
  }      
  
  num = cursor->c_get (cursor, &dbkey, &dbdata, DB_SET);
  if (num != 0)
  { cursor->c_close (cursor); return -1; }

  /* Then delete the record */

  num = cursor->c_del (cursor, 0);
  cursor->c_close (cursor);
  if (num != 0)
  {
    i_printf (1, "i_db_del found record to delete but calling c_del failed (%s)", db_strerror(num));
    return -1;
  }

  return 0;
}

