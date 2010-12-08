#include <stdlib.h>

#include <induction.h>

#include "feature.h"

extern i_list *global_feature_list;

/* Feature database functions */

int l_featuredb_put (i_resource *self, l_feature *feat)
{
  int num;
  int datasize;
  char *data;
  i_db *db;
  
  if (!self || !feat || !feat->id_str) return -1;

  data = l_feature_struct_to_data (feat, &datasize);
  if (!data || datasize < 1)
  { i_printf (1, "l_featuredb_put failed to convert feature to data"); return -1; }

  db = i_db_open (self, FEATURE_DB_FILE);
  if (!db)
  { i_printf (1, "l_featuredb_put failed to open %s", FEATURE_DB_FILE); free (data); return -1; }

  num = i_db_put (self, db, feat->id_str, strlen(feat->id_str)+1, data, datasize);
  i_db_free (db);
  free (data);
  if (num != 0)
  { i_printf (1, "l_featuredb_put failed to put record in db"); return -1; }

  return 0;
}

int l_featuredb_del (i_resource *self, char *id_str)
{
  int num;
  i_db *db;

  if (!self || !id_str) return -1;
  
  db = i_db_open (self, FEATURE_DB_FILE);
  if (!db)
  { i_printf (1, "l_featuredb_put failed to open %s", FEATURE_DB_FILE); return -1; }

  num = i_db_del (self, db, id_str, strlen(id_str)+1);
  i_db_free (db);
  if (num != 0)
  { i_printf (1, "l_featuredb_remove failed to remove record"); return -1; }

  return 0;
}


