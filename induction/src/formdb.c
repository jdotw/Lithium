#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "form.h"
#include "bdb.h"
#include "files.h"

/** \addtogroup form Web Forms
 * @{
 */

i_form* i_formdb_get (i_resource *self, char *form_name)
{
  i_db *db;
  i_db_record *record;
  i_form *form;

  if (!self || !form_name) return NULL; 

  db = i_db_open (self, FORM_DB_FILE);
  if (!db)
  { i_printf (1, "i_formdb_get failed to open %s", FORM_DB_FILE); return NULL; }

  record = i_db_get (self, db, form_name, strlen(form_name)+1);
  i_db_free (db);
  if (!record) { i_printf (2, "i_formdb_get failed to find form %s in %s", form_name, FORM_DB_FILE); return NULL; }

  form = i_form_data_to_form (record->data, record->datasize);
  i_db_free_record (record);
  if (!form) { i_printf (1, "i_formdb_get failed to convert data for form %s in %s to the form struct", form_name, FORM_DB_FILE); return NULL; }

  return form;
}

int i_formdb_put (i_resource *self, char *form_name, i_form *form)
{
  int num;
  int datasize;
  char *data;
  i_db *db;

  if (!self || !form_name || !form) return -1; 

  /* Convert the form to a data block */

  data = i_form_form_to_data (form, &datasize);
  if (!data) { i_printf (1, "i_formdb_put unable to compile form %s to data", form_name); return -1; }

  /* Open the DB */

  db = i_db_open (self, FORM_DB_FILE);
  if (!db)
  { i_printf (1, "i_formdb_put failed to open %s to store %s", FORM_DB_FILE, form_name); free (data); return -1; }
        
  /* Store the data block */
  
  num = i_db_put (self, db, form_name, strlen(form_name)+1, data, datasize);
  i_db_free (db);
  free (data);
  if (num != 0) { i_printf (1, "i_formdb_put unable to put form %s in %s", form_name, FORM_DB_FILE); return -1; }

  return 0;
}

int i_formdb_del (i_resource *self, char *form_name)
{
  int num;
  i_db *db;

  if (!self || !form_name) return -1;
  
  /* Open the DB */

  db = i_db_open (self, FORM_DB_FILE);
  if (!db)
  { i_printf (1, "i_formdb_del failed to open %s to remove %s", FORM_DB_FILE, form_name); return -1; }
        
  /* Request the delete */

  num = i_db_del (self, db, form_name, strlen(form_name)+1);
  i_db_free (db);
  if (num != 0) { i_printf (1, "i_formdb_del unable to delete form %s from %s", form_name, FORM_DB_FILE); return -1; }

  return 0;
}


/* @} */
