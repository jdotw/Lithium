#include <stdlib.h>

#include <induction.h>

#include "feature.h"

static i_list *static_feature_list;

/* Pointer/variable fetching */

i_list* l_feature_list ()
{ return static_feature_list; }

/* Feature list initialisation */

int l_feature_list_init (i_resource *self)
{
  /* Initialise and return the feature list  */

  int num;
  i_list *dblist;
  i_list *featlist;
  i_db *db;
  i_db_record *record;

  if (!self) return -1;

  db = i_db_open (self, FEATURE_DB_FILE);
  if (!db)
  { i_printf (1, "l_feature_list_init failed to open %s", FEATURE_DB_FILE); return -1; }

  dblist = i_db_get_all_list (self, db);
  i_db_free (db);
  if (!dblist)
  { i_printf (1, "l_feature_list_init failed to get all records from %s", FEATURE_DB_FILE); return -1; }

  static_feature_list = i_list_create ();
  if (!static_feature_list)
  { 
    i_printf (1, "l_feature_list_init failed to create featlist");
    i_list_free (dblist);
    return -1; 
  }
  i_list_set_destructor (static_feature_list, l_feature_free);

  for (i_list_move_head(dblist); (record=i_list_restore(dblist))!=NULL; i_list_move_next(dblist))
  {
    /* Loop for each feature in the DB */
  
    l_feature *feature;

    feature = l_feature_data_to_struct (record->data, record->datasize);
    if (!feature)
    { i_printf (1, "l_feature_list_init failed to convert feature \"%s\" from data to struct. continuing", record->key); continue; }

    num = l_feature_enable (self, feature);
    if (num != 0)
    { i_printf (1, "l_feature_list_init failed to enable feature %s (%s)\n", feature->id_str, feature->desc_str); }

    i_list_enqueue (static_feature_list, feature);
  }

  i_list_free (dblist);

  return 0;
}

