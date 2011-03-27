#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "user.h"
#include "bdb.h"
#include "list.h"
#include "files.h"
#include "auth.h"

/* Get all */

i_list* i_userdb_get_all (i_resource *self)
{
  i_db *db;
  i_db_record *record;
  i_list *records;
  i_list *users;

  if (!self) return NULL;

  db = i_db_open (self, USER_DB_FILE);
  if (!db)
  { i_printf (1, "i_userdb_get_all_list failed to open db"); return NULL; }

  records = i_db_get_all_list (self, db);
  i_db_free (db);
  if (!records)
  { i_printf (1, "i_userdb_get_all_list to get all user records from DB"); return NULL; }

  users = i_list_create ();
  if (!users)
  { i_printf (1, "i_userdb_get_all_list failed to create users list"); i_list_free (records); return NULL; }
  i_list_set_destructor (users, i_user_free);
  
  for (i_list_move_head(records); (record=i_list_restore(records))!=NULL; i_list_move_next(records))
  {
    /* Loop through each record */
    i_user *user;

    user = i_user_data_to_struct (record->data, record->datasize);
    if (!user)
    { i_printf (1, "i_userdb_get_all_list failed to decode a users data from the db. continuing"); continue; }

    i_list_enqueue (users, user);
  }

  i_list_free (records);

  return users;
}

